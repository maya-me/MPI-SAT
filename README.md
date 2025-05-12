# MPI - SAT

Acesta este un proiect C++ care implementează rezoluția, DP-ul și DPLL-ul din problema satisfiabilității booleene.

## Cum este așteptat imputul

Inputul trebuie să fie în format **DIMACS**:

Liniile de la începutul fișierului pot începe cu _c_, dar acestea vor fi tratate precum comentarii. Acest tip de rând nu este permis mai încolo.

Pe primul rând care nu este un comentariu se așteaptă, în această ordine:

- _p cnf_ - adică fișierul este scris în format cnf
- _numarul de variabile_ - pe același rând, numărul de variabile pe care le are formula
- _numărul de clauze_ - numărul de clauze pe care le are formula

Pe următoarele rânduri (atâtea cât numărul de clauze) se află clauzele formulei, care sunt așteptate astfel:
- literalii care se află în clauză, câte un număr diferit de 0 și aflat în intervalul [-_număr de clauze_, +_număr de clauze_]
- 0, la sfârșitul rândului, reprezentând ca s-a termină clauza

**Exemplu**:

Pentru formula (x₁ ∨ ¬x₂) ∧ (¬x₁ ∨ x₃) ∧ (x₂ ∨ ¬x₃ ∨ x₄), fișierul va arăta astfel:

p cnf 4 3

1 -2 0

-1 3 0

2 -3 4 0

## Rulare program

Pentru a utiliza programul, este necesar un compilator C++ instalat pe sistem, precum **g++**.

1. **Clonare repository-ul**:
   ```bash
   git clone https://github.com/maya-me/MPI-SAT

2. **Navigare în directorul proiectului**:
    ```bash
    cd nume-proiect

3. **Compilare proiect(cu compilatorul g++)**:


    ```bash
    g++ DPLL.cpp 

    //pentru a compila rezoluția sau DP-ul
    //se înlocuiește DPLL.cpp cu DP.cpp
    //sau resolution.cpp

4. **Rulare program:**
    ```bash
    ./a.exe
