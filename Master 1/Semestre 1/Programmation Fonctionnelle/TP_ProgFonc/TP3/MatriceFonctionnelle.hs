-- type matrice fonctionnelle

type Matf = Int -> Int -> (Bool, Int)

-- fonction representant la matrice de l'exemple
--    |  1  2  3  4  5
--  --|----------------
--  1 |  3  4  5  6  7
--  2 |  5  6  7  8  9
--  3 |  7  8  9 10 11
--  4 |  9 10 11 12 13
--  5 | 11 12 13 14 15
--  6 | 13 14 15 16 17 

exemple :: Matf
exemple i j
   | 1<=i && i<=6 && 1<=j && j<=5 = (True, 2*i+j)
   | otherwise = (False, 0)

-- fonction representant la matrice identite pour une matrice 4X4
 
identite4x4 :: Matf
identite4x4 i j
    | 1<=i && i<=4 && 1<=j && j<=4 && i == j = (True, 1)
    | 1<=i && i<=4 && 1<=j && j<=4 && i /= j = (True, 0)
    | otherwise = (False, 0)
 
-- fonction representant la matrice identite pour une matrice 6X5
 
identite6x5 :: Matf
identite6x5 i j
    | 1<=i && i<=6 && 1<=j && j<=5 && i == j = (True, 1)
    | 1<=i && i<=6 && 1<=j && j<=5 && i /= j = (True, 0)
    | otherwise = (False, 0)
 
 -----------------------------------------------
 --
 -- Recherche des dimensions d'une matrice
 --
 -----------------------------------------------

nbLines :: Matf -> Int
nbLines f = nbLinesIntern f 1
    where nbLinesIntern f i = case f i 1 of
                                (True,  _) -> 1 + nbLinesIntern f (i+1) 
                                (False, 0) -> 0

nbCols :: Matf -> Int
nbCols f = nbColsIntern f 1
    where nbColsIntern f j = case f 1 j of
                               (True,  _) -> 1 + nbColsIntern f (j+1) 
                               (False, 0) -> 0

dims :: Matf -> (Int, Int)
dims m = (nbLines m, nbCols m)

 -----------------------------------------------
 --
 -- Somme des matrices "mat_A" et "mat_B"
 --
 -----------------------------------------------
 
cmpDims :: Matf -> Matf -> Bool
cmpDims m n = dims m == dims n

add :: Matf -> Matf -> Matf
add m n = if not (cmpDims m n) 
            then error "add appliquée à 2 matrices de tailles différentes"
            else let (l, c) = dims m in
                 (\i j -> if 1 <= i && i <= l && 1 <= j && j <= c
                            then (True, snd (m i j) + snd (n i j))
                            else (False, 0))
