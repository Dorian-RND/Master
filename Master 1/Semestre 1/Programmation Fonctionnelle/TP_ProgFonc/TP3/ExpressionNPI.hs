evalElem ::(Num a, Read a) => [a] -> String -> [a]
evalElem (x:y:ys) "+" = (x+y):ys
evalElem (x:y:ys) "-" = (y-x):ys
evalElem (x:y:ys) "*" = (y*x):ys
evalElem xs e = (read e):xs

evalNPI :: (Num a, Read a) => String -> a
-- evalNPI e = head (foldl evalElem [] (words e))
evalNPI = head . foldl evalElem [] . words 