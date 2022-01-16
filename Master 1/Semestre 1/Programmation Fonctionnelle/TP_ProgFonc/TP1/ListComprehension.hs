sumcarre :: Int
sumcarre = sum [ x^2 | x <- [1..100] ]

replic :: Int -> a -> [a]
replic n a = [ a | _ <- [1..n]]

pyths :: Int -> [(Int, Int, Int)]
pyths n = [ (x,y,z) | x <- [1..n], y <- [1..n], z <- [1..n],
            z == n && x^2 + y^2  == z^2 || 
            y == n && x^2 + z^2  == y^2 || 
            x == n && z^2 + y^2  == x^2]