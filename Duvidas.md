# Duvidas

1. podemos adicionar 1 parametro na função llclose (int role?)





Procedure **llopen** (Tx side):

1. Tx sends command message to Rx (SET_C)
   1.1. set TIMEOUT timer

   1.2. if alarm goes off without receiving UA:

   - if `retry` has exceeded legal value exit with error.
   - increment `retry` Go back to step 1.

