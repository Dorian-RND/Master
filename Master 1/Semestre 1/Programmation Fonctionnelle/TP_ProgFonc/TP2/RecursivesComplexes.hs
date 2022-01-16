import Prelude hiding (drop, take)

paire :: Int -> Bool
paire 0 = True
paire n = impaire (n - 1)

impaire :: Int -> Bool
impaire 0 = False
impaire n = paire (n - 1)

drop :: Int -> [a] -> [a]
drop 0 xs = xs
drop _ [] = []
drop n (x:xs) = drop (n-1) xs 

take :: Int -> [a] -> [a]
take 0 xs = []
take _ [] = []
take n (x:xs) = x : take (n-1) xs

halve :: [a] -> ([a], [a])
halve [] = ([],[])
halve xs = (take n xs, drop n xs)
    where n = (length xs) `div` 2

merge :: Ord a => [a] -> [a] -> [a]
merge [] xs = xs
merge xs [] = xs
merge (x:xs) (y:ys) | x <= y = x : merge xs (y:ys)
                    | otherwise = y : merge (x:xs) ys

msort :: Ord a => [a] -> [a]
msort [] = []
msort [x] = [x]
msort xs = merge (msort rs) (msort ls)
    where (rs, ls) = halve xs