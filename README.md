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

### Spelen


### Bronnen
* https://zguide.zeromq.org/
* https://stackoverflow.com/
* https://github.com/cb92/Battleship