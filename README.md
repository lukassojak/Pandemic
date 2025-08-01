# Pandemic — Agent-based simulace šíření pandemie

Tento projekt je jednoduchou **agent-based simulací šíření viru** v populaci. Vstupem jsou data o **agentech** a **světě (místech)**, kde se pohybují, a parametry viru jsou zadávány jako **přepínače z příkazové řádky**.

## Spuštění programu

Program se spouští následovně:
```bash
./pandemic [OPTIONS] agents.csv world.csv
```

### Povinné argumenty
- `agents.csv` - CSV soubor s definicemi agentů
- `world.csv` - CSV soubor popisující místa (vrcholy úplného grafu)

### Volitelné přepínače
- `--lethality <float>`: Smrtnost viru (0-1). Výchozí hodnota: `0.5`
- `--infectivity <float>`: Infekčnost viru (0–1). Výchozí hodnota: `0.5`
- `--duration <float>`: Doba infekčnosti (0–1). Výchozí hodnota: `0.5`
- `--vaccine-modifier <float>`: Účinnost vakcíny (≥ 0). Výchozí hodnota: `1.2`
- `--max-steps <int>`: Maximální počet kroků simulace. Pokud není zadáno, běží simulace až do konce (všichni agenti se uzdraví nebo zemřou).
- `--random-seed <int>`: Seed pro generátor náhodných čísel (pro testování a reprodukovatelnost). Výchozí hodnota: `time(NULL)`
- `--summary`: Vypíše pouze finální štatistiku simulace (bez detailního výpisu kroků).
- `--verbose`: Zahrne do výstupu i hodnoty náhodných hodů použitých při simulaci.

> ⚠️ Přepínače `--summary` a `--verbose` se vzájemně vylučují. Pokud jsou zadány oba, program vypíše chybové hlášení na standardní chybový výstup a ukončí se.


## Struktura vstupních souborů

### world.csv — Definice světa

Soubor popisuje jednotlivá místa, kde se agenti mohou vyskytovat. Každý řádek reprezentuje jeden vrchol úplného grafu:
- `id` (int): Unikátní nezáporný identifikátor místa
- `name` (string): Název místa (max. délka min. 16 ASCII znaků)
- `exposure` (float): Nezáporná hodnota vyjadřující úroveň expozice viru na daném místě (vyšší hodnota = větší pravděpodobnost šíření nákazy)

#### Příklad:

```csv
id;name;exposure
0;Park;0.3
1;Škola;0.8
2;Obchod;0.6
```

### agents.csv - Definice agentů
- `id` (int): Unikátní identifikátor agenta
- `location` (int): ID místa, kde se agent nachází na začátku simulace (odkazuje na `id` z `world.csv`)
- `state` (char): Počáteční zdravotní stav agenta:
  - `S` — Susceptible (náchylný k nákaze)
  - `I` — Infected (nakažený)
  - `R` — Recovered (vyléčený)
  - `D` — Dead (mrtvý) — obvykle není použitý na začátku simulace

#### Příklad:

```csv
id;location;state
1;0;I
2;2;S
3;1;R
```

## Ukázky použití

Spuštění simulace s výchozími parametry:
```bash
./pandemic agents.csv world.csv
```

Spuštění s definovaným seedem, 100 kroky a jen souhrnem:
```bash
./pandemic --random-seed 42 --max-steps 100 --summary agents.csv world.csv
```

Ladění s výpisem náhodných hodů:
```bash
./pandemic --verbose --infectivity 0.7 --lethality 0.3 agents.csv world.csv
```
