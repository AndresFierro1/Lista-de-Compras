#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <set>

using namespace std;

struct Producto {
    string categoria;
    string nombre;
};

struct Cuenta {
    string usuario;
    string contrasena;
};

struct ProductoPospuesto {
    string usuario;
    string producto;
    string tienda;
};

// Cargar productos desde archivo
vector<Producto> cargarProductos(const string& archivo) {
    vector<Producto> productos;
    ifstream file(archivo);
    string linea;

    while (getline(file, linea)) {
        stringstream ss(linea);
        string categoria, nombre;
        getline(ss, categoria, ',');
        getline(ss, nombre);

        productos.push_back({ categoria, nombre });
    }

    file.close();
    return productos;
}

// Guardar productos en el archivo
void guardarProductos(const vector<Producto>& productos, const string& archivo) {
    ofstream file(archivo);
    for (const auto& producto : productos) {
        file << producto.categoria << "," << producto.nombre << endl;
    }
    file.close();
}

// Cargar productos pospuestos desde archivo
vector<ProductoPospuesto> cargarProductosPospuestos(const string& archivo) {
    vector<ProductoPospuesto> pospuestos;
    ifstream file(archivo);
    string linea;

    while (getline(file, linea)) {
        stringstream ss(linea);
        string usuario, producto, tienda;
        getline(ss, usuario, ',');
        getline(ss, producto, ',');
        getline(ss, tienda);

        pospuestos.push_back({ usuario, producto, tienda });
    }

    file.close();
    return pospuestos;
}

// Guardar productos pospuestos en el archivo
void guardarProductosPospuestos(const vector<ProductoPospuesto>& pospuestos, const string& archivo) {
    ofstream file(archivo);
    for (const auto& pospuesto : pospuestos) {
        file << pospuesto.usuario << "," << pospuesto.producto << "," << pospuesto.tienda << endl;
    }
    file.close();
}

// Posponer producto
void posponerProducto(vector<string>& carrito, vector<ProductoPospuesto>& pospuestos, const string& usuarioAutenticado, const string& tienda) {
    if (carrito.empty()) {
        cout << "Tu carrito está vacío. No hay productos para posponer.\n";
        return;
    }

    cout << "Productos en el carrito:\n";
    for (size_t i = 0; i < carrito.size(); ++i) {
        cout << i + 1 << ". " << carrito[i] << endl;
    }

    int seleccion;
    cout << "Seleccione el número del producto que desea posponer: ";
    cin >> seleccion;

    if (seleccion < 1 || seleccion > carrito.size()) {
        cout << "Selección no válida.\n";
        return;
    }

    string productoPosponer = carrito[seleccion - 1];
    carrito.erase(carrito.begin() + seleccion - 1);

    pospuestos.push_back({ usuarioAutenticado, productoPosponer, tienda });
    guardarProductosPospuestos(pospuestos, "pospuestos.txt");

    cout << "El producto '" << productoPosponer << "' ha sido pospuesto.\n";
}

// Mostrar productos pospuestos
void mostrarProductosPospuestos(const vector<ProductoPospuesto>& pospuestos, const string& usuarioAutenticado) {
    cout << "--- Productos Pospuestos ---\n";
    bool hayProductos = false;

    for (const auto& pospuesto : pospuestos) {
        if (usuarioAutenticado == "admin" || pospuesto.usuario == usuarioAutenticado) {
            cout << "Usuario: " << pospuesto.usuario
                << " | Producto: " << pospuesto.producto
                << " | Tienda: " << pospuesto.tienda << endl;
            hayProductos = true;
        }
    }

    if (!hayProductos) {
        cout << "No hay productos pospuestos para mostrar.\n";
    }
}

// Agregar categoría y producto
void agregarCategoria(vector<Producto>& productos, const string& usuarioAutenticado) {
    if (usuarioAutenticado != "admin") {
        cout << "Solo el administrador puede agregar categorías.\n";
        return;
    }

    string nuevaCategoria;
    cout << "Ingrese el nombre de la nueva categoría: ";
    cin.ignore();
    getline(cin, nuevaCategoria);

    string nuevoProducto;
    cout << "Ingrese el nombre del producto para esta categoría: ";
    getline(cin, nuevoProducto);

    productos.push_back({ nuevaCategoria, nuevoProducto });
    guardarProductos(productos, "productos.txt");
    cout << "Categoría y producto agregados correctamente.\n";
}

// Eliminar categoría
void eliminarCategoria(vector<Producto>& productos, const string& usuarioAutenticado) {
    if (usuarioAutenticado != "admin") {
        cout << "Solo el administrador puede eliminar categorías.\n";
        return;
    }

    string categoriaEliminar;
    cout << "Ingrese el nombre de la categoría a eliminar: ";
    cin.ignore();
    getline(cin, categoriaEliminar);

    productos.erase(remove_if(productos.begin(), productos.end(),
        [&categoriaEliminar](const Producto& p) { return p.categoria == categoriaEliminar; }),
        productos.end());

    guardarProductos(productos, "productos.txt");
    cout << "La categoría '" << categoriaEliminar << "' y sus productos han sido eliminados.\n";
}

// Función para cargar el carrito de un usuario desde el archivo
vector<string> cargarCarrito(const string& usuario, const string& tienda) {
    vector<string> carrito;
    ifstream movFile("movimientos.txt");
    string linea;

    while (getline(movFile, linea)) {
        stringstream ss(linea);
        string usuarioMovimiento, accion, producto, tiendaMovimiento;

        getline(ss, usuarioMovimiento, ',');
        getline(ss, accion, ',');
        getline(ss, producto, ',');
        getline(ss, tiendaMovimiento);

        if (usuarioMovimiento == usuario && tiendaMovimiento == tienda && accion == "Agrego") {
            carrito.push_back(producto);
        }
    }

    movFile.close();
    return carrito;
}

// Función para mostrar las categorías de productos y permitir elegir producto
void mostrarCategoriasYSeleccionarProducto(const vector<Producto>& productos, vector<string>& carrito, const string& usuario, const string& tienda) {
    set<string> categorias;

    for (const auto& producto : productos) {
        categorias.insert(producto.categoria);
    }

    cout << "\n--- Categorías de Productos ---\n";
    int categoriaIndex = 1;
    map<int, string> categoriasMap;
    for (const auto& categoria : categorias) {
        categoriasMap[categoriaIndex] = categoria;
        cout << categoriaIndex++ << ". " << categoria << endl;
    }

    int categoriaSeleccionada;
    cout << "Seleccione una categoría (ingrese el número): ";
    cin >> categoriaSeleccionada;

    if (categoriaSeleccionada < 1 || categoriaSeleccionada > categoriasMap.size()) {
        cout << "Selección no válida.\n";
        return;
    }

    string categoriaElegida = categoriasMap[categoriaSeleccionada];
    cout << "\nIndique el nombre del producto: ";
    cin.ignore();
    string productoSeleccionado;
    getline(cin, productoSeleccionado);

    carrito.push_back(productoSeleccionado);
    ofstream movFile("movimientos.txt", ios::app);
    if (movFile.is_open()) {
        movFile << usuario << ",Agrego," << productoSeleccionado << "," << tienda << endl;
        movFile.close();
    }
    cout << "Producto agregado: " << productoSeleccionado << endl;
}

// Función para cargar las cuentas desde el archivo
vector<Cuenta> cargarCuentas(const string& archivo) {
    vector<Cuenta> cuentas;
    ifstream file(archivo);
    string linea;

    while (getline(file, linea)) {
        stringstream ss(linea);
        string usuario, contrasena;
        getline(ss, usuario, ',');
        getline(ss, contrasena);

        cuentas.push_back({ usuario, contrasena });
    }

    file.close();
    return cuentas;
}

// Función para mostrar el carrito de un usuario
void mostrarCarrito(const string& usuario, const string& tienda, const string& usuarioAutenticado) {
    vector<string> carrito = cargarCarrito(usuario, tienda);

    if (usuarioAutenticado == "admin") {
        // Si el usuario autenticado es admin, muestra el carrito de todos los usuarios
        cout << "--- Carrito de " << usuario << " ---\n";
    }
    else if (usuario != usuarioAutenticado) {
        // Si el usuario no es el mismo que el autenticado, denegar el acceso
        cout << "Acceso denegado.\n";
        return;
    }

    if (carrito.empty()) {
        cout << "El carrito de " << usuario << " está vacío.\n";
        return;
    }

    cout << "Productos en el carrito de " << usuario << ":\n";
    for (size_t i = 0; i < carrito.size(); ++i) {
        cout << i + 1 << ". " << carrito[i] << endl;
    }
}

// Función para iniciar sesión
bool iniciarSesion(const vector<Cuenta>& cuentas, string& usuarioAutenticado, string& tiendaSeleccionada) {
    string usuario, contrasena;
    cout << "Ingrese su usuario: ";
    cin >> usuario;
    cout << "Ingrese su contraseña: ";
    cin >> contrasena;

    for (const auto& cuenta : cuentas) {
        if (cuenta.usuario == usuario && cuenta.contrasena == contrasena) {
            usuarioAutenticado = usuario;
            tiendaSeleccionada = "Tienda1";  // Se asume una tienda por defecto
            return true;
        }
    }

    cout << "Usuario o contraseña incorrectos.\n";
    return false;
}

// Función para cerrar sesión
void cerrarSesion(string& usuarioAutenticado) {
    usuarioAutenticado = "";
    cout << "Sesión cerrada.\n";
}

// Función principal
int main() {
    vector<Producto> productos = cargarProductos("productos.txt");
    vector<Cuenta> cuentas = cargarCuentas("cuentas.txt");
    vector<ProductoPospuesto> pospuestos = cargarProductosPospuestos("pospuestos.txt");
    string usuarioAutenticado, tiendaSeleccionada;

    cout << "--- Sistema de Carrito de Compras ---\n";


    if (!iniciarSesion(cuentas, usuarioAutenticado, tiendaSeleccionada)) {
        return 1;
    }

    int opcion;
    do {
        cout << "\n--- Menú ---\n";
        cout << "1. Mostrar categorías y seleccionar producto\n";
        cout << "2. Mostrar carrito\n";
        cout << "3. Posponer producto\n";
        cout << "4. Mostrar productos pospuestos\n";
        if (usuarioAutenticado == "admin") {
            cout << "5. Agregar categoría\n";
            cout << "6. Eliminar categoría\n";
            cout << "7. Mostrar todos los productos pospuestos\n";
            cout << "8. Posponer producto de cualquier usuario\n";
            cout << "9. Mostrar carritos de todos los usuarios\n";
            cout << "12. Desposponer producto\n";
            cout << "13. Proceder al pago\n";
        }
        cout << "10. Cerrar sesión\n";
        cout << "11. Salir\n";
        cout << "Seleccione una opción: ";
        cin >> opcion;

        switch (opcion) {
        case 1: {
            vector<string> carrito = cargarCarrito(usuarioAutenticado, tiendaSeleccionada);
            mostrarCategoriasYSeleccionarProducto(productos, carrito, usuarioAutenticado, tiendaSeleccionada);
            break;
        }
        case 2:
            mostrarCarrito(usuarioAutenticado, tiendaSeleccionada, usuarioAutenticado);
            break;
        case 3: {
            vector<string> carrito = cargarCarrito(usuarioAutenticado, tiendaSeleccionada);
            posponerProducto(carrito, pospuestos, usuarioAutenticado, tiendaSeleccionada);
            break;
        }
        case 4:
            mostrarProductosPospuestos(pospuestos, usuarioAutenticado);
            break;
        case 5:
            if (usuarioAutenticado == "admin") {
                agregarCategoria(productos, usuarioAutenticado);
            }
            else {
                cout << "Acceso denegado.\n";
            }
            break;
        case 6:
            if (usuarioAutenticado == "admin") {
                eliminarCategoria(productos, usuarioAutenticado);
            }
            else {
                cout << "Acceso denegado.\n";
            }
            break;
        case 7:
            if (usuarioAutenticado == "admin") {
                mostrarProductosPospuestos(pospuestos, "admin");
            }
            break;
        case 8:
            if (usuarioAutenticado == "admin") {
                cout << "Ingrese el nombre de usuario de la persona de la que desea posponer un producto: ";
                string usuario;
                cin >> usuario;
                vector<string> carrito = cargarCarrito(usuario, tiendaSeleccionada);
                posponerProducto(carrito, pospuestos, "admin", tiendaSeleccionada);
            }
            break;
        case 9:
            if (usuarioAutenticado == "admin") {
                for (const auto& cuenta : cuentas) {
                    mostrarCarrito(cuenta.usuario, tiendaSeleccionada, usuarioAutenticado);
                }
            }
            break;
        case 10:
            cerrarSesion(usuarioAutenticado);
            if (!iniciarSesion(cuentas, usuarioAutenticado, tiendaSeleccionada)) {
                return 1;
            }
            break;
        case 11:
            cout << "¡Gracias por utilizar el sistema!\n";
            break;
        case 12: // Desposponer producto
            if (usuarioAutenticado == "admin") {
                if (pospuestos.empty()) {
                    cout << "No hay productos pospuestos para desposponer." << endl;
                }
                else {
                    cout << "Selecciona el producto que deseas desposponer:" << endl;
                    for (size_t i = 0; i < pospuestos.size(); ++i) {
                        cout << i + 1 << ". Usuario: " << pospuestos[i].usuario
                            << ", Producto: " << pospuestos[i].producto
                            << ", Tienda: " << pospuestos[i].tienda << endl;
                    }

                    int seleccion;
                    cout << "Ingresa el número del producto a desposponer: ";
                    cin >> seleccion;

                    if (seleccion < 1 || seleccion > pospuestos.size()) {
                        cout << "Selección no válida." << endl;
                    }
                    else {
                        ProductoPospuesto productoSeleccionado = pospuestos[seleccion - 1];

                        ofstream movimientosFile("movimientos.txt", ios::app);
                        if (movimientosFile.is_open()) {
                            movimientosFile << productoSeleccionado.usuario << ","
                                << productoSeleccionado.producto << ","
                                << productoSeleccionado.tienda << endl;
                            movimientosFile.close();
                            cout << "Producto despospuesto y registrado en movimientos.txt." << endl;

                            pospuestos.erase(pospuestos.begin() + seleccion - 1);
                        }
                        else {
                            cout << "Error al abrir el archivo de movimientos." << endl;
                        }
                    }
                }
            }
            else {
                cout << "Acceso denegado.\n";
            }
            break;
        case 13: // Proceder al pago
            if (usuarioAutenticado == "admin") {
                ofstream movimientosFile("movimientos.txt", ios::trunc);
                if (movimientosFile.is_open()) {
                    movimientosFile.close();
                    cout << "Se pagó todo lo que está en el carrito." << endl;
                }
            }
            else {
                cout << "Acceso denegado.\n";
            }
            break;
        default:
            cout << "Opción no válida.\n";
        }
    } while (opcion != 11);
}










