function crearNuevaConexion(idSensor, typoSensor) {}

Conexion = {
  idSensor,// id del sensor
  tipoSensor,//tipo de sensor 
  mutex ,//para sincronizar el acceso a la lista de datos
  datos : []//lista dinamica para datos
}


let sensoresActivos = []//lista de conexiones activas
let logFile = open("log.file")//archivo para guardar los resultaodos


function aceptarNuevaConexion() {

  nuevaConexion = crearNuevaConexion(idSensor, typoSensor)
  sensoresActivos.add(nuevaConexion)//agregar

  //crear un thread para recibir datos de un sensor
  thread(escucharConexion, nuevaConexion)
}


function escucharConexion(args) {
  while (true) {
    let sensorConexion = args.nuevaConexion

    //leer un dato del socket
    let dato = leerDatoEnSocket()

    sensorConexion.mutex.lock()
    //agregar dato a la lista de datos recividos
    sensorConexion.datos.add(dato)
    sensorConexion.mutex.release()

    if (sensorConexion.tipoSensor == 0) {
      sleep(deltaT);//sperar deltaT antes de realizar los calculos
      let tid = thread(calcularMedicion)
      joinThread(tid);
      cleanBuffers();//remove los datos actuales para 

    }
  }
}

function calcularMedicion() {
  for ( let sensorActivo in sensoresActivos) {
    //ignorar sensor si no tiene Z datos
    if (sensorActivo.datos.length < Z) continue;
    sensorActivo.mutex.lock()
    let medicion = calcularValorReal(sensorActivo.datos)
    let incertidumbre = calcularIncertidumbre(sensorActivo.datos)
    sensorActivo.mutex.release()
  }

  let tupla = (0, 0, 0)
  //validar que haya mediciones de todos los tipos de sensor
  //combinacion lineal
  GuardarEnDisco(tupla)
}

function cleanBuffers(){
  //limpiar los valores que ya han sido procesador
  for ( let sensorActivo in sensoresActivos) {
    sensorActivo.mutex.lock()
    sensorActivo.datos.removeAll();//limpiar la lista
    sensorActivo.mutex.release()
  }

}

function GuardarEnDisco(tupla) {
  //guardar
}


Dato = {
  Valor: int,
  llegada: FechaHora
}
