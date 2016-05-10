(define-module (util)
  #:use-module (rnrs bytevectors)
  #:use-module (rnrs io ports)
  #:use-module (system foreign)
  #:use-module (ice-9 hash-table)
  #:export (progn list* prog1 typecase as-list pop push concat incf decf
                  concat-lit string-split string-strip print pprint
                  integer->bitvector bytevector-slice equal-any? foreach
                  keyword->symbol->string list->hashtable))
                   
                  


;;;;Macros
;;; Symbol aliases
(define-syntax progn
;;  "Alias for begin"
  (identifier-syntax begin))
(define-syntax list*
;;  "Alias for cons*"
  (identifier-syntax cons*))

;;;Control structures
(define-syntax prog1
;;  "Evaluate first and rest sequentally, return the value of first"
  (lambda (x)
    (syntax-case x ()
      ((_ first rest ...)
       #'(let ((ret first))
           rest ...
           first)))))
;;For what it's worth I tried to do this with syntax-case
(define-macro (typecase expr . clauses)
  (let ((val (gensym)))
    `(let ((,val ,expr))
       (cond
        ,@(map (lambda (x)
                (if (eq? #t (car x))
                    (list* #t (cdr x))
                    (list* (list (string->symbol
                                  (concat (symbol->string (car x)) "?")) val)
                           (cdr x)))) clauses)))))
;;;Function-like macros
(define-macro (as-list x)
  ;;  "If x is not a list return (list x) otherwise return x"
  (let ((val (gensym)))
    `(let ((,val ,x))
       (if (list? ,val) ,val (list ,val)))))
(define-syntax pop
  (lambda (x)
    (syntax-case x ()
      ((_ ls)
       #'(car (let ((ret ls))
                (set! ls (cdr ls))
                ret))))))
(define-syntax push
  (lambda (x)
    (syntax-case x ()
      ((_ elt place)
       #'(set! place (cons elt place))))))
(define-syntax decf
  (lambda (x)
    (syntax-case x ()
      ((_ y)
       #'(set! y (1- y))))))
(define-syntax incf
  (lambda (x)
    (syntax-case x ()
      ((_ y)
       #'(set! y (1+ y))))))
(define-macro (concat . strings)
  `(string-join (list ,@strings) ""))
;;Concatenate strings at macroexpansion time, allows writing long string
;;constants without looking ugly or suffering a performance penalty
(define-macro (concat-lit . strings)
  (string-join strings ""))
(define-macro (foreach var seq . body)
  `(for-each (lambda (x) (let ((,var x))
                      ,@body)) ,seq))

;;;;Utility Functions
;;;Sequence functions
(define (car-safe obj)
  (if (pair? obj) (car obj) '()))
(define (cdr-safe obj)
  (if (pair? obj) (cdr obj) '()))
;;;String Functions
(define (string-split str delim)
  "Split string into substrings delimited by delim.
Returns a list of substrings"
  (let acc ((index 0)
            (output '()))
    (let ((temp (string-index str delim (1+ index))))
      (if temp
          (acc temp (cons (substring str index temp) output))
          (reverse! (cons (substring str index) output))))))
(define (string-strip str)
  "returns a copy of str with leading and trailing whitespace removed"
  (let ((start (string-skip str char-set:whitespace))
        (end (string-skip-right str char-set:whitespace)))
    (substring str start (1+ end))))
;;;IO functions
(define* (print obj #:optional (port (current-output-port)))
  "write obj to port, followed by a newline"
  (write obj port)
  (newline port))
(define* (pprint obj #:optional (port (current-output-port)))
  "display obj on port, followed by a newline"
  (display obj port)
  (newline port))
;;;Type conversions/Value creation
(define (integer->bitvector n)
  "Convert an integer to a bitvector such that the most significant
bit in the integer is the first bit in the bitvector"
  (let* ((num-bits (integer-length n))
         (bit (1- num-bits))
         (vec (make-bitvector num-bits)))
    (while (> bit 0)
      (bitvector-set! vec (- num-bits 1 bit) (logbit? bit n))
      (decf bit))
    vec))

(define-inlinable (bytevector-slice bv start len)
  "Return a view into the bytevector bv starting at index 'start' for 'len' bytes"
  (if (or (> 0 start) (> 0 len)
          (> (+ start len) (bytevector-length bv)))
      (scm-error 'out-of-range "bytevector-slice"
                 "bv-len:~a, start:~a len:~a"
                 (list (bytevector-length bv) start len) #f))
  (let ((ptr (bytevector->pointer bv start)))
    (pointer->bytevector ptr len)))
(define-inlinable (bytevector->string bv start len)
  (utf8->string (bytevector-slice bv start len)))
(define (keyword->symbol->string key)
  "Return a string representation of the symbol with the same name as key"
  (symbol->string (keyword->symbol key)))
;; (define (list->hash-table ls)
;;   "Convert the list ls into a hash table.
;; elements of the list should have the form (key value)")


;;;Networking
(define (get-ip-addr host)
  "Convert a hostname into an ipv4 ip address"
  (sockaddr:addr (addrinfo:addr (car (getaddrinfo host)))))
(define* (connect-utf8-tcp host port)
  (when (string? host)
    (set! host (get-ip-addr host)))
  (let ((sock (socket PF_INET SOCK_STREAM 0)))
    (connect sock AF_INET host port)
    (set-port-encoding! sock "UTF-8")
    sock))


;;;Predicates
;;This should be a macro but its too hard to make it work
(define (equal-any? val args)
  (if (not (pair? args))
      (equal? val args)
      (let ((retval #f))
        (while (pair? args)
          (if (equal? val (pop args))
              (set! retval #t)
              break))
        retval)))
;;Define read syntax for hashtable literals of the form:
;;#,(hash (key value)...)
;;This is global so doesn't need to be exported, but instead
;;needs to be evalueated when this module is loaded
(eval-when (load)
  (use-modules (srfi srfi-10))
  (define-reader-ctor 'hash
    (lambda elems
      (let ((table (make-hash-table)))
        (for-each (lambda (elem)
                    (apply hash-set! table elem))
                  elems)
        table))))