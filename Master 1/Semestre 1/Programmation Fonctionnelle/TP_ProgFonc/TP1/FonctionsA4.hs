
egaux4 :: Eq a => a -> a -> a -> a -> Bool 
egaux4 a b c d = a==b && b==c && c==d

-- La fonction max (équivalente à max2) existe déjà
-- Exemple d'implémentation de max2 (== max)
max2 :: Ord a => a -> a -> a
max2 a b | a >= b = a
         | otherwise = b

max4 :: Ord a => a -> a -> a -> a -> a
max4 a b c d = max2 (max2 a b) (max2 c d)