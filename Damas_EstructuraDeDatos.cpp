#include <iostream>
#include <vector>

using namespace std;

enum class ficha {
    VACIA,
    BLANCA,
    DAMA_BLANCA,
    NEGRA,
    DAMA_NEGRA
};
//vamos a la playa oo oooo o
struct position {
    int fila, colu;

    bool operator==(const position& otra) const {
        return fila == otra.fila && colu == otra.colu;
    }
};
struct movimiento {
    position origen;
    position destino;
    vector<position> fichasComidas;
};
class tablero {

private:
    ficha casillas[8][8];
public:
    tablero() {
        inicializar();
    }

    void inicializar() {

        for (int f = 0; f < 8; f++)
            for (int c = 0; c < 8; c++)
                casillas[f][c] = ficha::VACIA;

        // Fichas negras
        for (int f = 0; f < 3; f++)
            for (int c = 0; c < 8; c++)
                if ((f + c) % 2 == 1)
                    casillas[f][c] = ficha::NEGRA;

        // Fichas blancas
        for (int f = 5; f < 8; f++)
            for (int c = 0; c < 8; c++)
                if ((f + c) % 2 == 1)
                    casillas[f][c] = ficha::BLANCA;
    }

    bool dentroDelTablero(int f, int c) const {
        return f >= 0 && f < 8 && c >= 0 && c < 8;
    }

    ficha obtener(position p) const {
        return casillas[p.fila][p.colu];
    }

    void colocar(position p, ficha f) {
        casillas[p.fila][p.colu] = f;
    }

    bool esBlanca(ficha f) const {
        return f == ficha::BLANCA ||
               f == ficha::DAMA_BLANCA;
    }

    bool esNegra(ficha f) const {
        return f == ficha::NEGRA ||
               f == ficha::DAMA_NEGRA;
    }

    bool esDama(ficha f) const {
        return f == ficha::DAMA_BLANCA ||
               f == ficha::DAMA_NEGRA;
    }

    bool colorContrario(ficha a, ficha b) const {

        if (a == ficha::VACIA || b == ficha::VACIA)
            return false;

        return (esBlanca(a) && esNegra(b)) ||
               (esNegra(a) && esBlanca(b));
    }

    void imprimir() const {

        cout << "\n    0   1   2   3   4   5   6   7\n";
        cout << "  +---+---+---+---+---+---+---+---+\n";

        for (int f = 0; f < 8; f++) {

            cout << f << " |";

            for (int c = 0; c < 8; c++) {

                char simbolo = ' ';

                switch (casillas[f][c]) {

                    case ficha::VACIA:
                        simbolo = ((f + c) % 2 == 1) ? '.' : ' ';
                        break;

                    case ficha::BLANCA:
                        simbolo = 'b';
                        break;

                    case ficha::DAMA_BLANCA:
                        simbolo = 'B';
                        break;

                    case ficha::NEGRA:
                        simbolo = 'n';
                        break;

                    case ficha::DAMA_NEGRA:
                        simbolo = 'N';
                        break;
                }

                cout << " " << simbolo << " |";
            }

            cout << "\n";
            cout << "  +---+---+---+---+---+---+---+---+\n";
        }

        cout << "b/B = blancas (normal/dama)   "
             << "n/N = negras (normal/dama)\n";
    }

    void intentarCoronar(position p) {

        ficha f = obtener(p);

        if (f == ficha::BLANCA && p.fila == 0) {

            colocar(p, ficha::DAMA_BLANCA);

            cout << "\n>> Una ficha blanca corono como DAMA en ("
                 << p.fila << "," << p.colu << ")\n";

        }
        else if (f == ficha::NEGRA && p.fila == 7) {

            colocar(p, ficha::DAMA_NEGRA);

            cout << "\n>> Una ficha negra corono como DAMA en ("
                 << p.fila << "," << p.colu << ")\n";
        }
    }

    int contarFichas(bool blancas) const {

        int contador = 0;

        for (int f = 0; f < 8; f++) {

            for (int c = 0; c < 8; c++) {

                ficha pieza = casillas[f][c];

                if (blancas && esBlanca(pieza))
                    contador++;

                if (!blancas && esNegra(pieza))
                    contador++;
            }
        }

        return contador;
    }
};

class Juego {

private:

    tablero tableroJuego;
    bool turnoBlancas;

    vector<pair<int, int>> direccionesDe(
        ficha pieza,
        const tablero& t
    ) {

        if (t.esDama(pieza))
            return {
                {-1, -1},
                {-1, 1},
                {1, -1},
                {1, 1}
            };

        if (pieza == ficha::BLANCA)
            return {
                {-1, -1},
                {-1, 1}
            };

        if (pieza == ficha::NEGRA)
            return {
                {1, -1},
                {1, 1}
            };

        return {};
    }

public:

    Juego() : turnoBlancas(true) {}
    void generarCapturasDesde(
        position origen,
        vector<position> capturadasPrevias,
        tablero tableroSim,
        vector<movimiento>& resultado
    ) {

        ficha pieza = tableroSim.obtener(origen);

        for (auto& d : direccionesDe(pieza, tableroSim)) {

            int fe = origen.fila + d.first;
            int ce = origen.colu + d.second;

            int fd = origen.fila + 2 * d.first;
            int cd = origen.colu + 2 * d.second;

            if (!tableroSim.dentroDelTablero(fd, cd))
                continue;

            position posEnemigo{fe, ce};
            position posDestino{fd, cd};


            bool yaCapturada = false;

            for (auto& p : capturadasPrevias) {

                if (p == posEnemigo) {
                    yaCapturada = true;
                    break;
                }
            }

            if (yaCapturada)
                continue;


            ficha fichaEnemiga =
                tableroSim.obtener(posEnemigo);

            ficha fichaDestino =
                tableroSim.obtener(posDestino);

            if (
                tableroSim.colorContrario(
                    pieza,
                    fichaEnemiga
                )
                &&
                fichaDestino == ficha::VACIA
            ) {

                tablero copia = tableroSim;

                copia.colocar(
                    origen,
                    ficha::VACIA
                );

                copia.colocar(
                    posEnemigo,
                    ficha::VACIA
                );

                copia.colocar(
                    posDestino,
                    pieza
                );


                vector<position> nuevasCapturadas =
                    capturadasPrevias;

                nuevasCapturadas.push_back(
                    posEnemigo
                );


                vector<movimiento> continuaciones;

                generarCapturasDesde(
                    posDestino,
                    nuevasCapturadas,
                    copia,
                    continuaciones
                );


                if (!continuaciones.empty()) {

                    for (auto& m : continuaciones)
                        resultado.push_back(m);

                }
                else {

                    movimiento mov{
                        origen,
                        posDestino,
                        nuevasCapturadas
                    };

                    resultado.push_back(mov);
                }
            }
        }
    }

    vector<movimiento> generarMovimientosSimples(
        position origen
    ) {

        vector<movimiento> movimientos;

        ficha pieza = tableroJuego.obtener(origen);

        for (auto& d :
             direccionesDe(pieza, tableroJuego)) {

            int nf = origen.fila + d.first;
            int nc = origen.colu + d.second;

            if (
                tableroJuego.dentroDelTablero(nf, nc)
                &&
                tableroJuego.obtener({nf, nc})
                    == ficha::VACIA
            ) {

                movimientos.push_back({
                    origen,
                    {nf, nc},
                    {}
                });
            }
        }

        return movimientos;
    }

    // Obtiene todos los movimientos legales
    vector<movimiento> obtenerMovimientosLegales() {

        vector<movimiento> todasCapturas;
        vector<movimiento> todosSimples;


        for (int f = 0; f < 8; f++) {

            for (int c = 0; c < 8; c++) {

                position pos{f, c};

                ficha pieza =
                    tableroJuego.obtener(pos);


                if (pieza == ficha::VACIA)
                    continue;


                if (
                    turnoBlancas &&
                    !tableroJuego.esBlanca(pieza)
                )
                    continue;

                if (
                    !turnoBlancas &&
                    !tableroJuego.esNegra(pieza)
                )
                    continue;


                vector<movimiento> capturas;


                generarCapturasDesde(
                    pos,
                    {},
                    tableroJuego,
                    capturas
                );


                for (auto& m : capturas)
                    todasCapturas.push_back(m);



                if (capturas.empty()) {

                    for (
                        auto& m :
                        generarMovimientosSimples(pos)
                    ) {
                        todosSimples.push_back(m);
                    }
                }
            }
        }


        if (!todasCapturas.empty())
            return todasCapturas;

        return todosSimples;
    }


    void ejecutarMovimiento(
        const movimiento& mov
    ) {

        ficha pieza =
            tableroJuego.obtener(mov.origen);



        tableroJuego.colocar(
            mov.origen,
            ficha::VACIA
        );



        for (auto& capturada :
             mov.fichasComidas) {

            tableroJuego.colocar(
                capturada,
                ficha::VACIA
            );
        }


        tableroJuego.colocar(
            mov.destino,
            pieza
        );



        tableroJuego.intentarCoronar(
            mov.destino
        );
    }


    bool hayGanador() {

        int blancas =
            tableroJuego.contarFichas(true);

        int negras =
            tableroJuego.contarFichas(false);


        if (blancas == 0) {

            cout << "\n*** GANAN LAS NEGRAS: "
                 << "las blancas se quedaron sin fichas ***\n";

            return true;
        }


        if (negras == 0) {

            cout << "\n*** GANAN LAS BLANCAS: "
                 << "las negras se quedaron sin fichas ***\n";

            return true;
        }

        if (obtenerMovimientosLegales().empty()) {

            cout << "\n*** GANAN LAS "
                 << (turnoBlancas ? "NEGRAS" : "BLANCAS")
                 << ": "
                 << (turnoBlancas
                     ? "las blancas"
                     : "las negras")
                 << " estan bloqueadas "
                 << "(sin movimientos) ***\n";

            return true;
        }
        return false;
    }


    void jugar() {

        cout << "===================================\n";
        cout << "          JUEGO DE DAMAS\n";
        cout << "===================================\n";

        cout << "Movimiento -> "
             << "Fila de columna, columna de la ficha "
             << "filaDestino, columna Destino\n"
             << "Como cordenadas\n";

        cout << "Ejemplo: 5 2 4 3\n";


        while (true) {

            tableroJuego.imprimir();


            if (hayGanador())
                break;


            cout << "\nTurno de las "
                 << (
                     turnoBlancas
                     ? "BLANCAS (b/B)"
                     : "NEGRAS (n/N)"
                    )
                 << "\n";


            vector<movimiento> legales =
                obtenerMovimientosLegales();


            if (
                !legales.empty()
                &&
                !legales[0].fichasComidas.empty()
            ) {

                cout << "Hay captura(s) disponible(s): "
                     << "es obligatorio comer.\n";
            }

            int fo, co, fd, cd;

            cout << "Tu movimiento: ";


            if (!(cin >> fo >> co >> fd >> cd)) {

                cout << "Entrada invalida.\n";
                break;
            }

            position origen{fo, co};
            position destino{fd, cd};

            movimiento* elegido = nullptr;

            for (auto& m : legales) {

                if (
                    m.origen == origen
                    &&
                    m.destino == destino
                ) {

                    elegido = &m;
                    break;
                }
            }

            if (!elegido) {

                cout << "\n*** Movimiento ilegal "
                     << "(revisa direccion, captura "
                     << "obligatoria, etc). ***\n";

                continue;
            }

            ejecutarMovimiento(*elegido);

            turnoBlancas = !turnoBlancas;
        }

        cout << "\nFin del juego.\n ";

    }
};

int main() {

    Juego juego;

    juego.jugar();

    return 0;
}