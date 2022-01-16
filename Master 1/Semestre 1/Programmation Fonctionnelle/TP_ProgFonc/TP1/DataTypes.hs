data Parfum = Chocolat
            | Vanille
            | Framboise
    deriving Show

prixParfum :: Parfum -> Double
prixParfum Chocolat = 1.5
prixParfum Vanille = 1.2
prixParfum Framboise = 1.4

data Glace = UneBoule Parfum
           | DeuxBoules Parfum Parfum
           | TroisBoules Parfum Parfum Parfum
    deriving Show

prixGlace :: Glace -> Double
prixGlace (UneBoule x) = 0.1 + prixParfum x
prixGlace (DeuxBoules x y) = sum (0.15 : (map prixParfum (x:y:[])))
prixGlace (TroisBoules x y z) = sum (0.20 : (map prixParfum (x:y:z:[])))