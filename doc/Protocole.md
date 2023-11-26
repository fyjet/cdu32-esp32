## Liste ey valeur des constantes
### Side Keys et Center Keys
LSK1 01 LCK1 11 RCK 21 RSK1 31  
LSK2 02 LCK1 12 RCK 22 RSK2 32  
LSK3 03 LCK1 13 RCK 23 RSK3 33  
LSK4 04 LCK1 14 RCK 24 RSK4 34  
LSK5 05 LCK1 15 RCK 25 RSK5 35  
LSK6 06 LCK1 16 RCK 26 RSK6 36  
LSK7 07 LCK1 17 RCK 27 RSK7 37  
LSK8 08 LCK1 18 RCK 28 RSK8 38  
### Couleurs
WHITE 99  
YELLOW 98  
GREEN 97  
BLACK 96  
BLUE 95  
GREY 94  

### Encodeurs
INC1UP 50  
INC1DN 51  
INC2UP 52  
INC2DN 53  
### Boutons mecaniques
BT1 57  
BT2 58  
BT3 59  

## bridge vers device (mode commande)
### Nouvel ecran
topic: j/[nomDevice]/c/d  
content: N,[titre]  
New Screen, efface l'ecran, supprime les boutons/reactions et ecrit titre en haut en blanc

### Creer un bouton clickable sur l'ecran
topic: j/[nomDevice]/c/d  
content: B,[code Side Key sur 2 car],[intutule bouton]  
cree un Bonton vert LSK1-4, RSK1-4, qui réagit aux appuis et retourn la valeur du bouton presse sur le topic key

### Affichage texte, avec ou sans contour clickable
topic: j/[nomDevice]/c/d  
content: T,XX,CC,B,Z,[chaine formattee]  
affichage du text chaine a l'emplacement Side Key XX de couleur CC  
si B=0, pas de contour, si B=1 contour vert
Z est la taille de la police: 2 ou 3

### afficher un slider
topic: j/[nomDevice]/c/d  
content: S,XX,CC,value in percent
afficher du slider a l'emplacement XX (LSK1-4, RSK1-4) de couleur CC, la valeur est en pourcent

### commander la led pilote automatique
topic: j/[nomDevice]/c/d  
content: L,value (0 or 1)
allume (value=1) ou eteint la led (value=0)

### demande status (facultatif)
topic: j/[nomDevice]/c/reqstatus  
content: 1  
envoie une demande de status

## device vers bridge (mode evennement)
### Touche pressee (clickable, mecanique ou encorder)
topic: j/[device]/e/k  
content: [valeur bouton presse]  
envoie le code de la touche pressee (valeur chaine constante boutons tactiles/mecaniques/encodeurs)

### status
topic: j/[device]/e/s  
content: [code status]  
en reponse au reqstatus, retourne le status actuel  
0 OK
autre ERREUR
