# Network_Programming_2022

## Zeeslag

Dit project bestaat uit een server en client waarop zeeslag kan gespeeld worden. Een gedeelte van de code voor specifiek het zeeslag spel komt van [cb92](https://github.com/cb92/Battleship). Deze spel code is aangepast om gebruikt te kunnen worden voor deze service. 


### Compilen

Het project moet met cmake gecompiled worden. De zmq library en de Qt6::core library worden ook gebruikt. 

### Uitvoeren
Open eerst de NP.exe (server) dan de NP_Client.exe (client). In de client kan dan een bericht verstuurd worden om te subscriben op Zeeslag. Hierna wordt te keuze weergeven om een lobby aan te maken of te joinen.
* Lobby aanmaken => 
  * Kies dan als speler 2 een mens is of een computer.
* Lobby joinen => 
  * Geef het lobbynummer nummer in om te joinen.


### Commands/Spelen
Client: [Zeeslag>create>13646>]
Server: [Zeeslag>13646>0>]
Client: [Zeeslag>0>user>13646>response>ready>]
Server: [Zeeslag>0>server>13646>print>Is player 2 Human (or CPU) y/n>request>bool>]
Client: [Zeeslag>0>user>15276>response>0>]
Server: [Zeeslag>0>server>15276>print>how would you like your board to be set? (enter 0 for non-auto, 1 for auto)>request>bool>]
Client: [Zeeslag>0>user>15276>response>1>]

Server: 
```c++
[Zeeslag>0>server>15276>print>Your starting board: >print>  
  A B C D E F G H I J
0 w w w w w w w w w _
1 w w w _ _ _ _ _ _ _
2 w w w w w w w w w w
3 w w w w w w w w w w
4 w w w _ _ _ _ w w w
5 _ _ _ _ _ w w w w w
6 w w w w w w w w w w
7 w w w w w w w w w w
8 w w w w w w w w w w
9 w w w w w w w w w w >]
```

Server: 
```c++ 
[Zeeslag>0>server>15276>print>player 1's GAME STATE:
>print>YOUR BOARD:
>print> 
A B C D E F G H I J
0 w w w w w w w w w _
1 w w w _ _ _ _ _ _ _
2 w w w w w w w w w w
3 w w w w w w w w w w
4 w w w _ _ _ _ w w w
5 _ _ _ _ _ w w w w w
6 w w w w w w w w w w
7 w w w w w w w w w w
8 w w w w w w w w w w
9 w w w w w w w w w w
>print>YOUR OPPONENT'S BOARD:
>print>  
  A B C D E F G H I J
0 ? ? ? ? ? ? ? ? ? ?
1 ? ? ? ? ? ? ? ? ? ?
2 ? ? ? ? ? ? ? ? ? ?
3 ? ? ? ? ? ? ? ? ? ?
4 ? ? ? ? ? ? ? ? ? ?
5 ? ? ? ? ? ? ? ? ? ?
6 ? ? ? ? ? ? ? ? ? ?
7 ? ? ? ? ? ? ? ? ? ?
8 ? ? ? ? ? ? ? ? ? ?
9 ? ? ? ? ? ? ? ? ? ?
>]
```

Client: [Zeeslag>0>user>15276>response>E4>]

Server: 
```c++ 
[Zeeslag>0>server>15276>print>player 1's GAME STATE:
>print>YOUR BOARD:
>print>  
  A B C D E F G H I J
0 w w w w w w w w w _
1 w w w _ _ _ _ _ _ _
2 w w w w w w w w w w
3 w w w w w w w w O w
4 w w w _ _ _ _ w w w
5 _ _ _ _ _ w w w w w
6 w w w w w w w w w w
7 w w w w w w w w w O
8 w w w w w w w w w w
9 w w w w w w w w w w
>print>YOUR OPPONENT'S BOARD:
>print>  
  A B C D E F G H I J
0 ? ? ? ? ? ? ? ? ? ?
1 ? ? ? ? ? ? ? ? ? ?
2 ? ? ? ? ? ? ? ? ? ?
3 ? ? ? ? ? ? ? ? ? ?
4 ? ? ? ? O ? ? ? ? ?
5 ? ? ? ? ? ? ? ? ? ?
6 ? ? ? ? ? ? ? ? ? ?
7 ? ? ? ? ? ? ? ? ? ?
8 ? ? ? ? ? ? ? ? ? ?
9 ? ? ? ? ? ? ? ? ? ?
>]
```

Client: ...
Server: ...


### Bronnen
* https://zguide.zeromq.org/
* https://stackoverflow.com/
* https://github.com/cb92/Battleship