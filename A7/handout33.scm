#lang racket



;; P1 sloved
(define (k-subsets set k) (if (zero? k) '(())
                              (if (> k (length set)) '()
                                  (append  (map (lambda (subset) (cons (car set) subset))
                                                                       (k-subsets (cdr set) (- k 1)) ) (k-subsets (cdr set) k) )
                              )
  ))

(k-subsets '(1 2 3 4) 3)
(k-subsets '(1 2 3 4 5 6) 5)
(k-subsets '(a b c d) 0)
(k-subsets '(a b c d) 5)

;; P2 a) sloved
(define (is-up-down? list comp) (if (< (length list) 2) #t
                                    (and (comp (car list ) (cadr list))
                                         (is-up-down? (cdr list) (lambda
                                                                     (fisrt second) (and (not (comp fisrt second))
                                                                                            (not (equal? fisrt second) )))
                                                     ) )
                                     
                                 ))
(is-up-down? '() <)
(is-up-down? '(1 2 3 4) <)
(is-up-down? '(1 2 2 4) <)
(is-up-down? '(1 6 2 4 3 5) <)
(is-up-down? '(4 8 3 5 1 7 6 9) <)


;; P2 b) sloved

(define (remove ls elem)
  (cond ((null? ls) '())
        ((equal? (car ls) elem) (remove (cdr ls) elem))
        (else (cons (car ls) (remove (cdr ls) elem)))))


(define (down-up-permute num-list)
  (if (< (length num-list) 2) (cons  num-list '())
      (apply append (map (lambda (elem) (remove-not-down-up-part (up-down-permute (remove num-list elem)) elem) )
               num-list)) 
      
  ))



(define (up-down-permute num-list)
  (if (< (length num-list) 2) (cons  num-list '())
      (apply append (map (lambda (elem) (remove-not-up-down-part (down-up-permute (remove num-list elem)) elem) )
               num-list)
      )
      )
  )



(define (remove-not-up-down-part list k)
  (if (null? list ) '()
      (if ( < k (car (car list))) (cons (cons k (car list)) (remove-not-up-down-part (cdr list) k))
                             (remove-not-up-down-part (cdr list) k)
      ))
  
  )


(define (remove-not-down-up-part list k)
  (if (null? list ) '()
      (if ( > k (car (car list))) (cons (cons k (car list)) (remove-not-down-up-part (cdr list) k))
                             (remove-not-down-up-part (cdr list) k)
      )
  )
)

(up-down-permute '())
(up-down-permute '(1))
(up-down-permute '(1 2 3))
(up-down-permute '(1 2 3 4 5))
