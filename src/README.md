

## Linux system admin

### Com posar un servei systemd en marxa periòdicament

1. On es posa el servei que fa la feina, i el timer que el crida periòdicament?

El *servei* i el fitxer *timer* es posen a la carpeta de linux `/etc/systemd/system`.

2. Estructura del servei. Pot ser un fitxer així molt simple:
```
[Unit]
Description="Get current daily percentile of stock values"

[Service]
ExecStart=/starter.sh
```
On simplement es posa una descripció de què fa, en el tag `[Unit]` i en el tag `[Service]` es dóna la localització del programa que s'executarà quan es cridi aquest servei.

3. Estructura del fitzer `timer`. Té una estructura així:

```
[Unit]
Description="run the get daily stock percentiles every day"

[Timer]
OnBootSec=5min
OnUnitActiveSec=24h
OnCalendar=*-*-* 12:00:00
Unit=getDailies.service

[Install]
WantedBy=multi-user.target
```
Aquí el tag `[Install]` és força important, perquè el `WantedBy` pot ser: `graphical.target`(inicia una interfaç d'usuari i el `multi-user.target`),  `sysinit.target`(al iniciar el systema) o `multi-user.target`(un servei que no necessita interfaç gràfica). Nosaltres triem l'últim encara que per defecte es posa el gràfical target.

L'altre element important és `OnCalendar` ja que aquí s'ha d'especificar en format especial, l'intèrval de periodicitat que volem per al nostre servei. Llegiu aquí alguns exemples de com posar el format de periodicitat : https://wiki.archlinux.org/title/systemd/Timers .

L'element `OnBootSec=5min` indica quant temps després de l'inici del systema, es vol executar el servei.

L'element `Unit`, indica quin servei executar. Ha d'estar a la mateixa carpeta que el *timer*.
 
 L'element `OnUnitActiveSec=24h` reinicia el servei després de 24h de l'ultima execució.

## Notes sobre la programació:

1. Al posar un for loop, i a dins un if(getline(fitxer, string, ",")) es comporta el getline malament, i llegeix 3 cops seguits la última linea si el for loop segueix en peu. 
En canvi si posem un while loop while(getline(fitxer, string, ",")) el getlien es fa adecuadament i no es repeteix la lectura de la última línia del fitxer. Fet extrany però cert.