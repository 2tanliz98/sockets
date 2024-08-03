Instrucciones para Compilar y Ejecutar el Proyecto


Servidor

El archivo servidor_.c debe compilarse y ejecutarse en el host del servidor. Utiliza los siguientes comandos para compilar y ejecutar:

gcc servidor_.c -o servidor
./servidor


Cliente

El archivo cliente.c debe compilarse y ejecutarse en el host del cliente. Utiliza los siguientes comandos para compilar y ejecutar:

gcc cliente.c -o cliente
./cliente


Base de Datos

El archivo alumnos.txt contiene la base de datos de los alumnos. Asegúrate de que este archivo esté presente en el directorio del servidor.


Archivo Auxiliar

El archivo basedatos.h se utiliza como archivo auxiliar para manejar las operaciones CRUD (Crear, Leer, Actualizar y Eliminar) en la base de datos.

Notas Importantes
- El proyecto acepta únicamente cadenas de texto en el formato especificado en el archivo de ejemplo proporcionado.
- El servidor se desconectará si se introduce una cadena vacía o caracteres no válidos que no estén definidos en el conjunto aceptado.