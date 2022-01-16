import Control.Applicative
import Data.Char

-------------------------------------------------------------------------------
-- Définitions de base

data Parser a = P (String -> [(a, String)])

item :: Parser Char
item = P (\s -> case s of
                    [] -> []
                    x:xs -> [(x,xs)])

parse :: Parser a -> String -> [(a, String)]
parse (P f) s = f s

-------------------------------------------------------------------------------
-- Instanciation comme Functor, Applicative et Monad

instance Functor Parser where
    -- fmap :: (a -> b) -> Parser a -> Parser b
    fmap f p = P (\s -> case parse p s of
                            [] -> []
                            [(r,s')] -> [(f r, s')])

instance Applicative Parser where
    -- pure :: a -> Parser a
    pure a = P (\s -> [(a, s)])
    
    -- <*> :: Parser (a -> b) -> Parser a -> Parser b
    pf <*> p = P (\s -> case parse pf s of
                            [] -> []
                            [(f,s')] -> parse (fmap f p) s')

instance Monad Parser where
    -- return :: a -> Parser a
    return = pure
    
    -- (>>=) :: Parser a -> (a -> Parser b) -> Parser b
    p >>= f = P (\s -> case parse p s of
                           [] -> []
                           [(r, s')] -> parse (f r) s')

-------------------------------------------------------------------------------
-- Parser à 3 étapes séquentielles

-- Définition classique
three' :: Parser (Char,Char)
three' = item >>= (\c -> item >>= (\_ -> item >>= (\e -> pure (c,e))))

-- Définition avec écriture simplifiée
three :: Parser (Char,Char)
three = do
          x <- item
          item
          y <- item
          return (x,y)

-------------------------------------------------------------------------------
-- Traitements alternatifs

instance Alternative Parser where
    -- empty :: Parser a
    empty = P (\_ -> [])

    -- (<|>) :: Parser a -> Parser a -> Parser a
    p1 <|> p2 = P (\s -> case parse p1 s of
                             [] -> parse p2 s
                             [(r, s')] -> [(r, s')])           

-------------------------------------------------------------------------------
-- Parser primitifs

sat :: (Char -> Bool) -> Parser Char
sat f = do c <- item
           if f c
               then return c
               else empty

digit :: Parser Char
digit = sat isDigit

lower :: Parser Char
lower = sat isLower

upper :: Parser Char
upper = sat isUpper

letter :: Parser Char
letter = sat isAlpha

alphanum :: Parser Char
alphanum = sat isAlphaNum

char :: Char -> Parser Char
char c = sat (== c)

string :: String -> Parser String
string [] = return []
string (x:xs) = do char x
                   string xs
                   return (x:xs)

-------------------------------------------------------------------------------
-- Parser évolués

ident :: Parser String
ident = do c <- lower
           xs <- many alphanum
           return (c:xs)

nat :: Parser Int
nat = do ds <- some digit
         return (read ds)

space :: Parser ()
space = do many (sat isSpace)
           return ()
           
int :: Parser Int
int = do char '-'
         n <- nat
         return (-n)
      <|> nat

-------------------------------------------------------------------------------
-- Parser d'expression arithmétiques

expr :: Parser Int
expr = do t <- term
          char '+'
          e <- expr
          return (t+e)
       <|> term

term :: Parser Int
term = do f <- factor
          char '*'
          t <- term
          return (f * t)
       <|> factor

factor :: Parser Int
factor = do char '('
            e <- expr
            char ')'
            return e
         <|> nat

eval :: String -> Int
eval s = case parse expr s of
            [(n,"")] -> n
            [(_,o)] -> error $ "Syntax error. Unable to parse " ++ o
            [] -> error "Syntax error"