# coding=utf-8

import matplotlib.pyplot as plt
import numpy as np

BATCHNUM = 128
STATISTICS = 11

def plotInfiniteHorizon(centro):
    if centro < 1 or centro > 5:
        print("Centro non valido...\n")
        return 1

    centers = ['ticket_machine', 'ticket_office', 
               'customer_support', 'security_check', 
               'ticket_gate']

    rho = []
    E_Nq = []
    E_N = []
    E_S = []
    E_Tq = []
    E_Ts = []
    interArrival = []
    user = []
    statistiche = []

    center = centers[centro - 1]

    statistiche.append(rho)
    statistiche.append(E_Nq)
    statistiche.append(E_N)
    statistiche.append(E_S)
    statistiche.append(E_Tq)
    statistiche.append(E_Ts)
    statistiche.append(interArrival)
    statistiche.append(user)

    filename = "../results/infinite/" + str(center) + ".csv"
    f = open(filename, "r")

    numeroBatch = 0
    stat = 0
    item1 = 0
    item2 = 0
    item3 = 0
    item4 = 0
    item5 = 0
    item6 = 0
    item7 = 0
    item8 = 0
    item9 = 0
    item10 = 0
    item11 = 0	
    count = 0

    for line in f:
        if line != "\n":
            rigaSplittata = line.split(";")
            if stat == 0:			
                for rho in rigaSplittata:
                    if rho != "\n":
                        item1 += 1
                        statistiche[stat].append(float(rho))			
            elif stat == 1:		
                for q in rigaSplittata:
                    if q != "\n":
                        item2 += 1
                        statistiche[stat].append(float(q))
            elif stat == 2:
                for n in rigaSplittata:
                    if n != "\n":
                        item3 += 1
                        statistiche[stat].append(float(n))
            elif stat == 3:
                for s in rigaSplittata:
                    if s != "\n":
                        item4 += 1
                        statistiche[stat].append(float(s))
            elif stat == 4:
                for d in rigaSplittata:
                    if d != "\n":
                        item5 += 1
                        statistiche[stat].append(float(d))
            elif stat == 5:
                for w in rigaSplittata:
                    if w != "\n":
                        item6 += 1
                        statistiche[stat].append(float(w))
            elif stat == 6:
                for r in rigaSplittata:
                    if r != "\n":
                        item7 += 1
                        statistiche[stat].append(float(r))
            elif stat == 7:
                for riga in rigaSplittata:
                    if riga != "\n":
                        item8 += 1
                        statistiche[stat].append(float(riga))
            elif stat == 8:
                for a in rigaSplittata:
                    if a != "\n":
                        item9 += 1
                        statistiche[stat].append(float(a))
            elif stat == 9:
                for np in rigaSplittata:
                    if np != "\n":
                        item10 += 1
                        statistiche[stat].append(float(np))
            elif stat == 10:
                for p in rigaSplittata:
                    if p != "\n":
                        item11 += 1
                        statistiche[stat].append(float(p))

            count += 1
            numeroBatch += 1
            if numeroBatch % BATCHNUM == 0:
                stat += 1

    f.close()  # Chiudere il file dopo l'uso

    # Visualizzazione Grafici
    fontLabel = {'color': 'black', 'size': 14}  # Ridotto a 14
    fontTitle = {'color': 'black', 'size': 18}  # Ridotto a 18
    FONTNUM = 14  # Ridotto a 14
    
    for i, y in enumerate(statistiche):
        plt.plot(y, color='blue')
        plt.xticks(fontsize=FONTNUM)
        plt.yticks(fontsize=FONTNUM)
        plt.ylabel(["ρ", "E[Nq]", "E[Ns}", "E[S]", "E[Tq]", "E[Ts]", "Interarrivi", "Utenti"][i], fontdict=fontLabel)
        plt.xlabel("Batch", fontdict=fontLabel)
        plt.title(["Utilizzazione", "Numero di Jobs in coda", "Numero di Job nel centro",
                   "Tempo di servizio", "Tempo in coda", "Tempo di risposta", 
                   "Interarrivi", "Arrivi"][i], fontdict=fontTitle)
        
        # Salva il grafico
        plt.savefig(f'plot_infinite_{centro}_{i}.png', bbox_inches='tight')
        #plt.show()

    return 0

# Definizione del secondo plot

REPLICATIONS = 128
SAMPLESIZE = 168

def plotFiniteHorizon(centro):
    if centro < 1 or centro > 5:
        print("Centro non valido...\n")
        return 1

    utilizzazioni = []
    numeroCoda = []
    numeroCentro = []
    servizi = []
    attese = []
    risposte = []
    interarrivo = []
    arriviFam = []
    arriviAuto = []
    nmPerdite = []
    probDiPerdita = []
    statistiche = [utilizzazioni, numeroCoda, numeroCentro, servizi, attese, risposte, interarrivo, arriviFam, arriviAuto, nmPerdite, probDiPerdita]

    # Costruzione struttura dati per la manipolazione dei dati
    for stat in range(0, STATISTICS):
        statistiche[stat] = []
        for replica in range(0, REPLICATIONS):
            statistiche[stat].append([])

    filename = "../data/finite/servicenodesampling" + str(centro) + ".dat"
    f = open(filename, "r")

    replica = [0] * REPLICATIONS

    count = 0

    for line in f:
        if line != "\n":
            rigaSplittata = line.split(";")
            idx = count % STATISTICS
            for val in rigaSplittata:
                if val != "\n":
                    statistiche[idx][replica[idx]].append(float(val))
            replica[idx] += 1
            count += 1

    f.close()  # Chiudere il file dopo l'uso

    # Campioni
    stats = [[] for _ in range(STATISTICS)]

    # Costruzione dei campioni
    for stat in range(0, STATISTICS):
        for value in range(0, SAMPLESIZE):
            media = sum(statistiche[stat][rep][value] for rep in range(REPLICATIONS)) / REPLICATIONS
            stats[stat].append(media)

    # Costruisco il grafico
    fontLabel = {'color': 'black', 'size': 14}  # Ridotto a 14
    fontTitle = {'color': 'black', 'size': 18}  # Ridotto a 18
    FONTNUM = 14  # Ridotto a 14

    for i, y in enumerate(stats):
        plt.plot(y, color='r')
        plt.xticks(fontsize=FONTNUM)
        plt.yticks(fontsize=FONTNUM)
        plt.ylabel(["ρ", "E(Nq)", "E(Ns)", "ES", "Tq", "Ts", "r", "F", "A", "NmPerdite", "ProbDiPerdita"][i], fontdict=fontLabel)
        plt.xlabel("Campione", fontdict=fontLabel)
        plt.title(["UTILIZZAZIONE", "NUMERO JOBS IN CODA", "NUMERO JOBS NEL CENTRO",
                   "TEMPO DI SERVIZIO", "TEMPO DI ATTESA", "TEMPO DI RISPOSTA", 
                   "INTERARRIVO", "ARRIVI FAMIGLIE", "ARRIVI AUTOMOBILI", 
                   "NUMERO PERDITE", "PROBABILITA' DI PERDITA"][i], fontdict=fontTitle)
        
        # Salva il grafico
        plt.savefig(f'plot_finite_{centro}_{i}.png', bbox_inches='tight')
        #plt.show()

    return 0

plotInfiniteHorizon(1)
plotInfiniteHorizon(2)
plotInfiniteHorizon(3)
plotInfiniteHorizon(4)
plotInfiniteHorizon(5)
