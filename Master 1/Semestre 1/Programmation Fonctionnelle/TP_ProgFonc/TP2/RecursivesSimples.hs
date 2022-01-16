import Prelude hiding (product, length, init, (++))

product :: Num a => [a] -> a
product [] = 1
product (n:ns) = n * product ns

length :: [a] -> Int
length [] = 0
length (_:xs) = 1 + length xs

init :: [a] -> [a]
init [_] = []
init (x:xs) = x : init xs 

(++) :: [a] -> [a] -> [a]
[] ++ ys = ys
(x:xs) ++ ys = x : (xs ++ ys)

insert :: Ord a => a -> [a] -> [a]
insert x [] = [x]
insert x (y:ys) | x <= y = x : y : ys
                | otherwise = y : (insert x ys)

isort :: Ord a => [a] -> [a]
isort [] = []
isort (x:xs) = insert x (isort xs)
