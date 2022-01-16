-- Prend une liste de type a et retourne une liste de type a
qsort :: Ord a => [a] -> [a]
-- 1ère équation : retourne liste vide si appliquée sur une liste vide
qsort [] = []
-- 2ème équation : retourne une liste triée
qsort (x : xs) = qsort smaller ++ [x] ++ qsort larger
    where smaller = [a | a <- xs, a <= x]
          larger   = [a | a <- xs, a > x]
