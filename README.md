# Parallel-OR  

Problem Statement: 
A1 A2 A3.......Ai computations result in boolean values.
  We have to calculate A1|A2|A3.....|Ai.
  If any of the task gives true then we need to terminate further computation.

Communication within the processes in case a process gets a true as a result.

  -Scheduling the computations given P nodes and C cores per node.

  -How to provide an interface for ""PARTITIONING THE DATA"" in case computation
   is skewed SPMD model.

  -For MPMD Model provide an interface for ""DYNAMIC LOAD BALANCING"".


  Present the design solution with at least two examples for each part.

  Interface for message passing should be MPI.
  For shared memory, use OpenMP or Pthreads.

Answer to all the questions posed above can be found in DesignDocument.pdf .
