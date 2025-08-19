# commit after e0e6ce31
- [TEST] -> set des pulses sur deux moteurs en mode synchrones  -> OK
- [TEST] -> set une séquences de pulses > à la capacité du buffer -> OK
        la queue est traité et rempli au fu et à mesure qu'il y a de la place 
- [TEST] -> set des pulses et trigger un En Stopp, les pulses sont bien dropped et
            les motors ne sont pas bloquées.