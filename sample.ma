[top]
components : Scanner Interpreter Correction
in : lote fin
out : tidleScan tidleInterpret tidleVerify 
Link : lote lote@Scanner
Link : fin fin@Scanner
Link : tidle@Scanner tidleScan
Link : doc@Scanner doc@Interpreter
Link : finished@Scanner fin@Interpreter
Link : tidle@Interpreter tidleInterpret
Link : docv@Interpreter doc@Correction
Link : finished@Scanner fin@Correction
Link : tidle@Correction tidleVerify

[Scanner]
components : s@Scan sq@Queueproc
in : lote fin
out : doc tidle finished
Link : fin fin@s
Link : lote in@sq
Link : tomarlote@s done@sq
Link : empty@sq colavacia@s
Link : out@sq lote@s
Link : tidle@s tidle
Link : doc@s doc
Link : finished@s finished

[s]
tiempo_carga_cabecera : 0:0:5:0
tiempo_proc_doc : 0:0:0:750

[sq]
preparation : 0:0:3:0

[Interpreter]
components : i@Interpret iq@Queueproc igr@Generatorproc

in : doc fin
out : docv doct tidle cantdocv cantdoct
Link : doc in@iq
Link : fin fin@i
Link : empty@iq stop@igr
Link : arrancar@i stop@igr
Link : docv@i docv
Link : doct@i doct
Link : tidle@i tidle
Link : cantdocv@i cantdocv
Link : cantdoct@i cantdoct
Link : out@iq doc@i
Link : empty@iq colavacia@i
Link : out@igr rechazo@i
Link : docv@i done@iq
Link : doct@i done@iq

[iq]
preparation : 0:0:2:0

[i]
tiempo_interpret : 0:0:2:0
porc_rechazo : 10

[igr]
distribution : normal
mean : 10
deviation : 10

[Correction]
components : v@Verify vq@Queueproc vgr@Generatorproc
in : doc fin
out : doct tidle cantdocb cantdoct
Link : doc in@vq
Link : fin fin@v
Link : out@vgr rechazo@v
Link : empty@vq colavacia@v
Link : out@vq doc@v
Link : doct@v done@vq
Link : empty@vq stop@vgr
Link : arrancar@v stop@vgr
Link : doct@v doct
Link : tidle@v tidle
Link : cantdocb@v cantdocb
Link : cantdoct@v cantdoct

[vq]
preparation : 0:0:10:0

[v]
tiempo_verify : 0:0:10:0
porc_rechazo : 2
tiempo_busqueda : 0:2:0:0

[vgr]
distribution : normal
mean : 100
deviation : 100