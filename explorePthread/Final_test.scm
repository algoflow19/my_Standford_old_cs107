#lang racket

;;5
(define (marry people-list) (if (null? people-list ) '()
                                (if (null? (cdr people-list)) people-list
                                    (cons (list (car people-list) (cadr people-list) ) (marry (cddr people-list)) )
                                    )
                                )
  )
(marry '("Adam" "Eve" "George" "Martha" "Albert" "Costello"))
(marry '("Fred" "Wilma"))
(marry '())
(marry '("Lucy" "Schroeder" "Patty" "Linus" "Charlie Brown"))
(marry '("INFP"))

(define (map-everywhere mapFun elems-list) (if (null? elems-list) '()
                                        (if (list? (car elems-list))
                                            (cons (map-everywhere mapFun (car elems-list)) (map-everywhere mapFun (cdr elems-list) ) )
                                            (cons (mapFun (car elems-list)) (map-everywhere mapFun (cdr elems-list)) )
                                            )
                                        ))

(map-everywhere (lambda (x) (+ x 1)) '(1 (2 (3) 4 (5 (6))) (8 9)))
(map-everywhere list '("a" ("b" ("c" ("d" "e" "f" "g")))))




;;6
(define (longest-common-prefix first-list second-list) (if (or (null? first-list) (null? second-list)) '()
                                                           (if (equal? (car first-list) (car second-list))
                                                               (cons (car first-list) (longest-common-prefix (cdr first-list) (cdr second-list)) )
                                                               '()
                                                               )
                                                           ))
(longest-common-prefix '(a b c) '(a b d f))
(longest-common-prefix '(s t e r n) '(s t e r n u m)) 
(longest-common-prefix '(1 2 3) '(0 1 2 3))

(define (mdp fun list) (if (null? list) '()
                           (cons (fun list) (mdp fun (cdr list)) )
                           ))

(mdp length '(w x y z))
(mdp cdr '(2 1 2 8))


(define (ratedPoints-comp< p1 p2) (< (car p1) (car p2) ))

(define (partition pivot num-list comp)
  (if (null? num-list) '(() ())
      (let ((split-of-rest (partition pivot (cdr num-list) comp )))
	(if (comp (car num-list) pivot)
	    (list (cons (car num-list) (car split-of-rest)) (cadr split-of-rest))
	    (list (car split-of-rest) (cons (car num-list) (car (cdr split-of-rest))))))))



(define (quicksort num-list comp)
  (if (<= (length num-list) 1) num-list
      (let ((split (partition (car num-list) (cdr num-list) comp )))
	(append (quicksort (car split) comp) 
		(list (car num-list)) 
		(quicksort (cadr split) comp)))))

(define (length-comp l1 l2) (> (length l1) (length l2)) )

(define (longest-common-sublist list1 list2)  (car  (quicksort
                                                   (apply append
                                                          (mdp  (lambda (list1-sublist)
                                                                  (mdp (lambda (list2-sublist) (longest-common-prefix list1-sublist list2-sublist)) list2 )
                                                                  ) list1)
                                                          )
                                                   length-comp)) 
  )

(longest-common-sublist '(a b d f g j) '(b c d f g h j k))
(longest-common-sublist '(a b c d) '(x y)) 
(longest-common-sublist '(4 5 6 7 8 9 10) '(2 3 4 5 6 7 8))

