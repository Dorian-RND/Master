inverse :: [a] -> [a]
inverse [] = []
inverse (x:xs) = inverse xs ++ [x] 

isPalindrome :: Eq a => [a] -> Bool
isPalindrome [_] = False
isPalindrome xs = xs == inverse xs

doPalindrome :: [a] -> [a]
doPalindrome xs = init xs ++ inverse xs
