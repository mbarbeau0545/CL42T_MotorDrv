# test
## test 1
-regarder le signal dir avec la diode -> normalement ok mais pas sur 

-pulse/en/diag vérifié avec les diodes -> ok 

-regarder le inhib time avec un changement de direction -> ok

-pulse error max ->

-CL42T ligne 825 verifier car ya une erreur 


## test 2
- Verifier que les pulses s'arrêtent lorsqu'on set stopPulse_b à True  (done )
- vérifier que si l'on envoie plus de 65535 pulses d'un coup; ils s'envoie bien tout (pour vérifier on peut set CST_MAX_UINT_16BIT à 2  par exemple) PENSEZ A LE REMETTRE EHIN; (done)
- vérifier que si l'on set des pulses et que l'on change la fréquence pendant les pulses cela est bien pris en compte.