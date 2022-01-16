data Prop = Const Bool
          | Var Char
          | Not Prop
          | And Prop Prop
          | Imply Prop Prop

p1 :: Prop
p1 = And (Var 'A') (Not (Var 'A'))
          
p2 :: Prop
p2 = Imply (And (Var 'A') (Var 'B')) (Var 'A')
          
p3 :: Prop
p3 = Imply (Var 'A') (And (Var 'A') (Var 'B'))
          
p4 :: Prop
p4 = Imply (And (Var 'A') (Imply (Var 'A') (Var 'B'))) (Var 'B')

type Assoc k v = [(k,v)]
type Subst = Assoc Char Bool

find :: Eq k => k -> Assoc k v -> v
find k l = head [ v | (k',v) <- l, k == k' ]

eval :: Subst -> Prop -> Bool
eval _ (Const b) = b
eval s (Var x) = find x s
eval s (Not p) = not (eval s p)
eval s (And p q) = eval s p && eval s q
eval s (Imply p q) = eval s p <= eval s q

rmdups :: Eq a => [a] -> [a]
rmdups [] = []
rmdups (x:xs) = x : filter (/= x) (rmdups xs)

vars :: Prop -> [Char]
vars p = rmdups (vars2 p)

vars2 :: Prop -> [Char]
vars2 (Const _) = []
vars2 (Var x) = [x]
vars2 (Not p) = vars2 p
vars2 (And p q) = vars2 p ++ vars2 q
vars2 (Imply p q) = vars2 p ++ vars2 q 

bools :: Int -> [[Bool]]
bools 0 = [[]]
bools n = map (False :) bss ++ map (True :) bss
    where bss = bools (n-1)

substs :: Prop -> [Subst]
substs p = map (zip vs) (bools (length vs))
    where vs = vars p

isTaut :: Prop -> Bool
isTaut p = and [ eval s p | s <- substs p ]