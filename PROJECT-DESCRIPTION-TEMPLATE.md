# Mali Princ i Zvezdani Sistem

mi23069 - Čedomir Đorđević  
Scena inspirisana Malim princom, knjižicom autora Antoana de Sent-Egziperija. Inicijalno nisam zeleo da dodam fenjerdziju i njegov asteroid s obzirom da nije u istom solarnom sistemu (ili zvezdanom nebitno) kao asteroid malog princa. Sve modele/teksture/skyboxove sam radio sam jer sam hteo da budu kao od papira napravljeni da bih se drzao decije estetike originalne knjige ali kad sam dosao do fenjerdzije vec sam malo odustao od potpune vizuelne skladnosti.
## Controls

A -> Prebacivanje kamere na asteroid Malog Princa (zaustavlja rotaciju) / Povratak u pregled zvezdanog sistema  
D -> Prebacivanje kamere na asteroid sa fenjerom (zaustavlja rotaciju) / Povratak u pregled zvezdanog sistema  
Left / Right / Up / Down (strelice) -> Manuelna promena boje svetlosti fenjera (žuta, narandžasta, plava, zelena, ljubičasta, bela)  
C -> Uključivanje / isključivanje automatske rotacije boja fenjera jedna boja na svakih 0.6 sekundi
Scroll (točkić miša) -> Podešavanje intenziteta svetlosti fenjera (Spotlight)  
ESC -> gasi aplikaciju  

## Features

### Fundamental:

[x] Model with lighting
[x] Two types of lighting with customizable colors and movement through GUI or ACTIONS
    - Point light: Tačkasto svetlo u centru sunca koje obasjava asteroide i objekte na njima tokom rotacije
    - Spot light: Usmereni reflektorski izvor svetla postavljen na fenjer, sa promenom boje (strelice za manuelno menjanje,'C' za automatsku promenu svakih 0.6s) i podesivom jacinom preko točkića miša
[x] {ACTION_X} --- AFTER_M_SECONDS---Triggers---> {EVENT_A} ---> AFTER_N_SECONDS---Triggers---> {EVENT_B}
    - Pritisak na taster 'D' (prelazak kamere na fenjer asteroid) --- NAKON 2 SEKUNDE ---Triggers---> Treperenje svetlosti fenjera (flicker efekat) ---> NAKON 1.2 SEKUNDE ---Triggers---> Stabilizacija svetlosti na normalan intenzitet

### Group A:

[ ] Frame-buffers with post-processing   
[ ] Off-screen Anti-Aliasing  
[ ] Parallax Mapping
[ ] Bloom with the use of HDR

### Group B:

[ ] Deferred Shading  
[ ] Point Shadows  
[ ] SSAO

### Engine improvement:

[ ] ...

## Models:

- Asteroid model
- Mali Princ
- Lisica
- Fenjerdzija+asteroid+model (sve je jedan model)
- Sunce1 model
- Sunce2 model

## Textures:

- Teksture modela (asteroid, fenjerdzija, lisica, mali princ, sunce1, sunce2)
- Skybox svemir
