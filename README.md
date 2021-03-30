# Halprogbead
Simulation of SIRD and SEIRD type of disease spreads

check wiki for more info on the models: https://en.wikipedia.org/wiki/Compartmental_models_in_epidemiology

The model can simulate any kind of model, it realy only depends on 3-4 variables, but I used it mainly for SIRD simulations.

It can reproduce herd immunity and logistic curves that real life diseases have.

It is not exact though, its an agent based toy model, the number of variables is only 5. 

It has agents with states and positions, those infect eachother and also move each turn if possible. 

The output is in a file, it contains the number of new infected and new deaths next to each other in a line, accessing it isnt that hard.

If there are multiple runs at the same variables, the runs will be in different line, if a parameter is changed a new file will be made

Have to change every big parameter manually, sorry for that
