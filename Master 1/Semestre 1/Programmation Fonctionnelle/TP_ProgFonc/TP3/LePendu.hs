import System.IO

getCh :: IO Char 
getCh = do hSetEcho stdin False
           x <- getChar
           hSetEcho stdin True
           return x 

sgetLine :: IO String 
sgetLine = do c <- getCh
              if c == '\n'
                then do putChar c
                        return []
                else do putChar '-'
                        xs <- sgetLine 
                        return (c:xs)

match :: String -> String -> String 
match xs ys = [if elem x ys then x else '-' | x <- xs]

play :: String -> IO ()
play word = do putStr "? "
               guess <- getLine
               if guess == word
                    then putStrLn "You got it"
                    else do putStrLn (match word guess)
                            play word       

hangman :: IO ()
hangman = do putStrLn "Think of a word:"
             word <- sgetLine
             putStrLn "Try to guess it:"
             play word

main = hangman