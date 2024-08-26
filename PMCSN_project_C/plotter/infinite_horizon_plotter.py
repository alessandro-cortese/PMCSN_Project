import matplotlib.pyplot as plt
import csv
def single_plotter(center, i):

    current_stats = []

    filename = "../results/infinite/" + str(center) + ".csv"

    if i == 0:
        start = 0
        end = 128
        x_name = "rho"
        title = "Utilizzazione " + center.replace('_', ' ')
    elif i == 1:
        start = 130
        end = 257
        x_name = "E[Nq]"
        title = "Numero di Jobs in coda " + center.replace('_', ' ')
    elif i == 2:
        start = 259
        end = 386
        x_name = "E[N]"
        title = "Numero di Jobs nel servizio " + center.replace('_', ' ')
    elif i == 3:
        start = 388
        end = 515
        x_name = "E[S]"
        title = "Tempo medio di servizio " + center.replace('_', ' ')
    elif i == 4:
        start = 517
        end = 644
        x_name = "E[Tq]"
        title = "Tempo medio di attesa in coda " + center.replace('_', ' ')
    elif i == 5:
        start = 646
        end = 773
        x_name = "E[Ts]"
        title = "Tempo medio di servizio " + center.replace('_', ' ')
    elif i == 6:
        start = 775
        end = 902
        x_name = "interArrival"
        title = "Interarrivi " + center.replace('_', ' ')
    elif i == 7:
        start = 904
        end = 1031
        x_name = "Arrivi"
        title = "Arrivi degli Utenti " + center.replace('_', ' ')

    # Apri il file CSV e leggi i valori
    with open("../results/infinite/" + str(center) + ".csv", 'r') as file:
        righe = file.readlines()[start:end]

    current_stats = [float(riga.replace(';', '').replace('\n', '').replace(',', '.')) for riga in righe]


    #print(current_stats)

    # Impostazioni del font per le etichette e il titolo
    fontLabel = {'color': 'black', 'size': 14}  # Ridotto a 14
    fontTitle = {'color': 'black', 'size': 14}  # Ridotto a 14
    FONTNUM = 14  # Ridotto a 14

    # Generazione del grafico
    plt.plot(current_stats, color='blue')
    plt.xticks(fontsize=FONTNUM)
    plt.yticks(fontsize=FONTNUM)
    plt.ylabel(x_name, fontdict=fontLabel)
    plt.xlabel("Batch", fontdict=fontLabel)
    plt.title(title, fontdict=fontTitle)

    # Salva il grafico
    plt.savefig(f'./{center}/slot_2/plot_infinite_{center}_{i}.png', bbox_inches='tight')
    plt.close()
    #plt.show()

for i in range(0,8):
    single_plotter("ticket_machine", i)
    single_plotter("ticket_office", i)
    single_plotter("customer_support", i)
    single_plotter("security_check", i)
    single_plotter("ticket_gate", i)