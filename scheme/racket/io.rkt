#lang racket/base
(require "util.rkt")
(require "ffi/mem.rkt")
(require (rename-in racket/contract (-> -->)))

;;;Functions to convert racket ports to rnrs ports, since racket doesn't
;;;have a native input/output port type.
;;We need to use a contract since we're going to do something
;;unsafe, which would cause a segfault on a wrong type
(define/contract (scheme-port-sub-type port)
  (--> port? symbol?)
  (ptr-ref (cast port _scheme _pointer) _scheme 'abs
           scheme-output-port-sub-type-offset))
(define (port-sub-type port)
  ;;The symbol returned by scheme-port-sub-type isn't interned so we need
  ;;to create an interned symbol with the same name as it.
  (let ((subtype
         (symbol->string
          (scheme-port-sub-type port))))
    (case subtype
      (("<string-input-port>" "<string-output-port>") '<string-port>)
      (("<file-input-port>" "<file-output-port>") '<file-port>)      
      (("<stream-input-port>" "<stream-output-port>") '<stream-port>)
      (("<user-input-port>" "<user-output-port>") '<user-port>)
      (("<pipe-input-port>" "<pipe-output-port>") '<pipe>)
      (("<null-output-port>") "<null-port>")
      (("<tcp-input-port>" "<tcp-output-port>") '<tcp-port>)
      (("<console-input-port>") '<console-port>))))
;;This would be a lot eaiser if I could just get definations internal to
;;ports.rkt, but I don't know how to do that, and I've spent way too long
;;trying to figure out how

;;I already have the subtype when I call this so I may as well use it
(define (make-position-funs port subtype)
  (if (or (eq? subtype '<string-port>)
          (eq? subtype '<file-port>)
          (eq? subtype '<stream-port>))
      (values (lambda () (file-position port))
              (lambda (pos) (file-position port pos)))
      (values #f #f)))
(define (make-read-fun input)
  (lambda (bv start count)
    (let ((nbytes
           (read-bytes! bv input start (+ start count))))
      (if (eof-object? nbytes) 0 nbytes))))
(define (make-write-fun output)
  (lambda (bv start count)
    (write-bytes-avail bv output start (+ start count))))
(define (make-binary-input/output-port id input output subtype)
  (let-values (((get-pos set-pos) (make-position-funs input subtype))
               ((read) (make-read-fun input))
               ((write) (make-write-fun output))
               ((close) (lambda ()
                          (close-output-port output)
                          (close-input-port input))))
    (make-custom-binary-input/output-port id read write get-pos set-pos close)))
(define (make-binary-input-port id input subtype)
  (let-values (((get-pos set-pos) (make-position-funs input subtype))
               ((read) (make-read-fun input))
               ((close) (lambda () (close-input-port input))))
    (make-custom-binary-input-port id read get-pos set-pos close)))
(define (make-binary-output-port id output subtype)
  (let-values (((get-pos set-pos) (make-position-funs output subtype))
               ((write) (make-write-fun output))
               ((close) (lambda () (close-output-port output))))
    (make-custom-binary-output-port id read get-pos set-pos close)))
(define (ports->r6rs-port input (output #f))
  (--> port? port? port?)
  (let ((subtype (port-sub-type input)))
    (cond
     (output
      (if (not (and (input-port? input)
                    (output-port? output)
                    (eq? subtype (port-sub-type output))))
        (raise-argument-error 'ports->r6rs-port
                              "Invalid port types"
                              "input" input "output" output)
        (make-binary-input/output-port "<input/output port>"
                                       input output subtype)))
     ((input-port? input)
      (make-binary-input-port "<input port>" input subtype))
     ((output-port? input)
      (make-binary-output-port "<output port>" input subtype)))))
(define (read-delim read-fn port msgbuf delim)
  (let ((nbytes (read-fn port msgbuf)))
    (if (eq? (bytevector-u8-ref msgbuf (1- nbytes)))
        (bytevector-copy (bytevector-slice msgbuf 0 nbytes))
        (let* ((buf (bytevector-copy msgbuf))
               (buflen nbytes))
          ;;loop until we end a read on delim
          (while (not (eq? (bytevector-u8-ref buf (1- buflen)) delim))
            (set! nbytes (read-fn port msgbuf))
            (when (< (bytevector-length buf) (+ buflen nbytes))
              (set! buf (bytevector-extend buf (bytevector-length buf))))
            ;;dest offset source count
            (memcpy (bytevector->pointer buf) buflen
                    (bytevector->pointer msgbuf) nbytes)
            (set! buflen (+ nbytes buflen)))
          (bytevector-slice buf 0 buflen)))))
;;;ffi stuff
(define _socklen _int)
(define-cstruct _addrinfo
  ((ai_flags _int) (ai_family _int)
   (ai_socktype _int) (ai_protocol _int)
   (ai_addrlen _socklen) (ai_addr _sockaddr-pointer)
   (ai_cannonname _bytes) (ai_next _addrinfo-pointer)))
(define _sock-data (_array _uint8 16))
(define-cstruct _sockaddr
  ((sa_family _ushort) (sa_data _sock-data)))
(define-cstruct _in_addr
  ((s_addr _uint32)))
(define-cstruct _sockaddr_in
  ((sin_port _uint16) (_sin_addr _in_addr)));;plus some padding
(define-ffi-binding base-lib
  "scheme_get_port_fd" (_fun _scheme -> _intptr))
(define-ffi-binding base-lib
  "getaddrinfo" (_fun _bytes _bytes _addrinfo-pointer _pointer -> _int))
(require racket/provide)
(provide (except-out (all-defined-out)
                     (matching-identifiers-out
                      #rx"make-.*-funs?" (all-defined-out))))