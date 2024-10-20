#!/bin/false

nanecisto_example() {
	$exe --random-seed 42 --summary "$cli_data/agents/sample.csv" "$cli_data/worlds/village.csv"
}

nanecisto_example_with_args() {
	$exe --random-seed 1337 --summary --lethality 0.8 "$cli_data/agents/sample.csv" "$cli_data/worlds/village.csv"
}

nanecisto_bigger_example() {
	$exe --random-seed 145 --summary "$cli_data/agents/bigger.csv" "$cli_data/worlds/bigger.csv"
}

run -esucc -1 nanecisto_example.out 'Vzorové vstupy' nanecisto_example
run -esucc -1 nanecisto_example_with_args.out 'Vzorové vstupy s argumentami navyše' nanecisto_example_with_args
run -esucc -1 nanecisto_bigger_example.out 'Väčší vstup' nanecisto_bigger_example

