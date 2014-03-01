;;Just as a warning, this is all pretty inefficent as I have to work around
;;the fact that there's no such thing as a cons cell in clojure
(declare cons? car cdr)
(defmacro progn [& rest] `(do ~@rest))
(defmacro cl-if [test then & else]
  `(if ~test
    ~then
    (do ~@else)))
(defmacro raise [err] `(throw (Exception. ~(str err))))
(definterface Internal_Cons
  (internal_car [])
  (internal_cdr [])
  (internal_set_car [x])
  (internal_set_cdr [x]))
(deftype Cons [^{:volatile-mutable true} car
               ^{:volatile-mutable true} cdr]
  Internal_Cons
  (internal_car [_] car)
  (internal_cdr [_] cdr)
  (internal_set_car [this x] (set! car x))
  (internal_set_cdr [this x] (set! cdr x))
  Object
  (toString [this]
    (let [acc (fn [cell s]
                 (if (cons? (.internal_cdr cell))
                   (recur (.internal_cdr cell) (str s (.internal_car cell) " "))
                   (if (.internal_cdr cell)
                     (str s (.internal_car cell) " . " (.internal_cdr cell) ")")
                     (str s (.internal_car cell) ")"))))]
      (acc this "("))))
(defn cons? [cell] (instance? Cons cell))
(defn cons [x y] (Cons. x y))
(defn car [cell] (if (cons? cell)
                   (.internal_car cell)
                   (raise "type error in car")))
(defn cdr [cell] (if (cons? cell)
                   (.internal_cdr cell)
                   (raise "type error in cdr")))
(defn safe-car [cell] (if (cons? cell)
                        (.internal_car cell)
                        nil))
(defn safe-cdr [cell] (if (cons? cell)
                        (.internal_cdr cell)
                        nil))
(defn caar [cell]
  (car (car cell)))
(defn cadr [cell]
  (car (cdr cell)))
(defn cdar [cell]
  (cdr (car cell)))
(defn cddr [cell]
  (cdr (cdr cell)))
(defn caaar [cell]
  (car (car (car cell))))
(defn cdaar [cell]
  (cdr (car (car cell))))
(defn caadr [cell]
  (car (car (cdr cell))))
(defn cdadr [cell]
  (cdr (car (cdr cell))))
(defn caddr [cell]
  (car (cdr (cdr cell))))
(defn cdddr [cell]
  (cdr (cdr (cdr cell))))
(defn cddar [cell]
  (cdr (cdr (car cell))))
(defn cadar [cell]
  (car (cdr (car cell))))
(defn set-car! [cell x] (if (cons? cell)
                          (.internal_set_car cell x)
                          (raise "type error in set-car!")))
(defn set-cdr! [cell x] (if (cons? cell)
                          (.internal_set_cdr cell x)
                          (raise "type error in set-cdr!")))
(defn push! [x cell]
  (let [temp (cons (car cell) (cdr cell))]
    (set-cdr! cell temp)
    (set-car! cell x))
  cell)
(defn pop! [cell]
  (let [temp (car cell)]
    (set-car! y (car (cdr y)))
    (set-cdr! y (cdr (cdr y)))
    temp))
(defn rev [cell]
  (let [acc (fn [ls sl]
              (if (cons? ls)
                (recur (cdr ls) (cons (car ls) sl))
              (if ls
                (cons ls sl)
                sl)))]
    (acc cell nil)))
(defn rev* [cell last]
  (let [acc (fn [ls sl]
              (if (cons? ls)
                (recur (cdr ls) (cons (car ls) sl))
              (if ls
                (cons ls sl)
                sl)))]
    (acc cell last)))
(defn clj-list-to-cons [ls]
  (let [acc (fn [ls cell]
              (if (not-empty ls)
                (recur (rest ls) (cons (first ls) cell))
                (rev cell)))]
    (acc ls nil)))
(defn clj-list-to-cons* [ls]
  (let [acc (fn [ls cell]
              (if (not-empty (rest ls))
                (recur (rest ls) (cons (first ls) cell))
                (rev* cell (first ls))))]
    (acc ls nil)))
(defn list
;  ([a] (cons a nil))
;  ([a b] (cons a (cons b nil)))
;  ([a b c] (cons a (cons b (cons c nil))))
  [& rest]
  (clj-list-to-cons rest))

(defn list* [& rest]
  (clj-list-to-cons* rest))
(defn last [cell]
  (if (cons? (cdr cell))
    (recur (cdr cell))
    cell))
(defn copy-cons [cell]
  (let [acc (fn [old new]
              (if (cons? old)
                (recur (cdr old) (cons (car old) new))
                (rev* new old)))]
    (acc (cdr cell) (car cell))))
(defn append [x y]
  (let [z (copy-cons x)]
    (set-cdr! (last z) y)
    z))
(defn append-rest-internal [args]
  (let [acc (fn [args acc]
              (if args
                (recur (cdr args) (append acc (car args)))
                acc))]
    (acc (cdr args) (car args))))
(defn append-rest [& rest]
  (append-rest-internal (clj-list-to-cons rest)))
(defn append! [x y]
  (set-cdr! (last x) y))
;(defn list [& rest]
;  (let [acc (fn [
(defmacro defun [name args & body]
  `(defn ~name ~(vec args) ~@body))
(defmacro cl-defmacro [name args & body]
  `(defmacro ~name ~(vec args) ~@body))
(defmacro lambda [args & body]
  `(fn ~(vec args) ~@body))
(defmacro as-list [l]
  (if (list? l) l (clojure.core/list l)))
;man this is excessive, but I really wanted to avoid sequences as munch as possible
(defun flatten-1 (ls)
  (letfn [(acc1 [ls sl]
            (if (not-empty ls)
              (recur (rest ls) (conj sl (first ls)))
              sl))
          (acc2 [ls sl]
            (if (not-empty ls)
              (if (list? (first ls))
                (recur (rest ls) (acc2 (first ls) sl))
                (trampoline acc1 (rest ls) (conj sl (first ls))))
              sl))]
    (reverse (acc2 (rest ls) (as-list (first ls))))))
(defmacro defun [name args & body]
  `(defn ~name ~(vec args) ~@body))

(defmacro cl-let [bindings & body]
  `(let [~@(flatten-1 bindings)]
     ~@body))
(defmacro flet [bindings & body]
  `(let [~@(flatten-1 bindings)]
     ~@body))
(defmacro cl-if [test then & else]
  `(if ~test
    ~then
    (do ~@else)))
(defmacro progn [& rest] `(do ~@rest))
(defmacro prog1 [first & rest]
  `(let [ret# ~first]
     (do ~@rest)
     ret#))
(defmacro prog2 [fst snd & rest]
  `(do ~fst
       (let [ret# ~snd]
         (do ~@rest)
         ret#)))
(defun eq [x y] (identical? x y))
(defun equal [x y] (= x y))
(defmacro do [vars test & body]
  ;add vars later
  ;do ((var init step)...) (end-test result...) body...)
  (cl-let ((var (clj-list-to-cons vars))) 
          `(loop [~(car var) ~(cadr var)]
             (cl-if ~end-test
                    nil;add result later
                    ~@body
                    (recur (~(caddr step) ~(car var)))))))
     
