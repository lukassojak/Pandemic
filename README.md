# Pandemic

**Agent-based pandemic simulator implemented in C**

Pandemic is a simple simulation of disease spread in a population of moving agents.  
Each agent moves between locations (defined in a world file), may become infected, recover, or die.  
Simulation parameters such as infectivity, lethality, duration, and vaccine efficiency are configurable from the command line.

---

## Features

- Agent-based model with discrete time steps  
- Parameterized virus model (infectivity, lethality, duration, vaccine modifier)  
- Reproducible runs via random seed  
- Two modes of output:
  - `--summary` — concise simulation summary
  - `--verbose` — step-by-step log including random rolls
- Deterministic behavior for identical seeds and inputs  
- Simple CSV input format for agents and world configuration  

---

## Build

```bash
git clone https://github.com/lukassojak/Pandemic.git
cd Pandemic
cmake -S . -B build
cmake --build build
```

This produces the executable `pandemic` in the `build` directory:
```bash
/build/pandemic
```

---

## Usage

```bash
./build/pandemic [options] <agents_file> <world_file>
```

### Options

| Option               | Argument         | Description                                      | Default        |
|----------------------|------------------|--------------------------------------------------|----------------|
| `--lethality`        | `<float>` (0-1)  | Probability of death when infected               | `0.5`          |
| `--infectivity`      | `<float>` (0-1)  | Probability of infection upon contact            | `0.5`          |
| `--duration`         | `<float>` (0-1)  | Probability of recovery at each step             | `0.5`          |
| `--vaccine-modifier` | `<float>`  >0    | Reduction in infection probability if vaccinated | `1.2`          |
| `--max-steps`        | `<int>`    >0    | Maximum number of simulation steps               | Not limited    |
| `--random-seed`      | `<int>`          | Seed for random number generator                 | `time(NULL)`   |
| `--summary`          |                  | Compact summary output                           | Disabled       |
| `--verbose`          |                  | Extend output with random rolls                  | Disabled       |

> *Note:* Only one of `--summary` or `--verbose` can be used at a time.

---

## Example

### Input Files
**world.csv**
Each line represents a location with the following fields:
```csv
id;name;exposure
```

- `id`: Unique identifier for the location (non-negative integer)
- `name`: Name of the location (string, supported length up to 16 ASCII characters)
- `exposure`: Affecting factor for infection probability at this location (float >= 0)

Example:
```csv
1;Home;0.5
2;Work;3.8
3;Park;1.0
4;Supermarket;3.0
```

**agents.csv**
Each line represents an agent with the following fields:
```csv
id;route;is_vaccinated;immunity;is_infected
```

- `id`: Unique identifier for the agent (non-negative integer)
- `route`: Dash-separated list of location IDs representing the agent's movement path. After reaching the end, the agent loops back to the start. All location IDs must exist in the world file.
- `is_vaccinated`: Vaccination status (1 for vaccinated, 0 for not vaccinated)
- `immunity`: Initial immunity level (float between 0 and 1)
- `is_infected`: Infection status at the start (1 for infected, 0 for not infected)

Example:
```csv
1;4-1-3;1;0.5;0
2;5-1-3;1;0.5;0
3;5;1;0.2;0
4;4;0;0.2;1
5;5-2-3-1-3;0;0.5;1
```

> *Note:* In both CSV files, fields are separated by semicolons (`;`) and header lines are not expected.

### Run
```bash
./build/pandemic --infectivity 0.3 --verbose examples/agents2.csv examples/world2.csv
```

## License
This project is licensed under the MIT License © 2025 Lukáš Soják