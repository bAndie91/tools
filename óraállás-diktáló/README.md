
# Közmű mérőóra-állás bejelentő

## Alföldvíz

### Invokáció

```
./alfoldviz <felhasználási_hely_azonosító> <mérőóra_gyári_száma> <email> <óraállás>
```

### Kilépési kód

- `0`: adatok beküldve
- `1`: diktálás nem lett jóváhagyva

## NKM Áram

### Invokáció

```
./nkmaram <felhasználó-azonosító> <jelszó> <mérőállás-1> [<mérőállás-2> ...]
```

Ha több órád is van, a weboldalon megjelenő sorrendben sorold fel az óraállásokat (ami valószínűleg a gyári szám szerinti növekvő sorrend).

### Kilépési kód

- `0`: diktálás sikerült
- `1`: a programm hibára futott önellenőrzés során (weboldal-interakció szinten)
- `2`: rossz paraméterezés
- `3`: weboldal nem töltött be
- `4`: a programm váratlan hibára futott (javascript szinten)
- `5`: diktálás nem sikerült

## NKM Földgáz

### Invokáció

```
./nkmfoldgaz <felhasználó-azonosító> <mérőóra-gyári-szám> <mérőállás> [<email>] [<dátum>]
```

Az *email* mező alapértéke "ugyfel@nkmfoldgaz.hu", a *dátum*é az aktuális nap.

### Kilépési kód

lásd **NKM Áram**-nál

## Függőségek

- bash
- curl
- phantomjs, tesztelt verzió: 2.1.1
