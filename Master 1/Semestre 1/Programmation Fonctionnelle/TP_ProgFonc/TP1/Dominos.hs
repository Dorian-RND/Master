type Domino = (Int, Int)

dominosA2Match :: Domino -> Domino -> Bool
dominosA2Match (x,y) (w,z) = y == w || y == z || x == w || x == z

dominos2Construct :: Domino -> Domino -> Domino
dominos2Construct (x,y) (w,z) 
    | y == w = (x,z)
    | y == z = (x,w)
    | x == w = (y,z)
    | x == z = (y,w)

dominosA3Match :: Domino -> Domino -> Domino -> Bool
dominosA3Match d1 d2 d3 =
    (dominosA2Match d1 d2 && dominosA2Match d3 (dominos2Construct d1 d2))
    || (dominosA2Match d2 d3 && dominosA2Match d1 (dominos2Construct d2 d3))
    || (dominosA2Match d1 d3 && dominosA2Match d2 (dominos2Construct d1 d3))
    
d1 = (2,3) :: Domino
d2 = (2,4) :: Domino
d3 = (1,3) :: Domino
d4 = (1,2) :: Domino