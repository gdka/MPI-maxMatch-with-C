#!/bin/bash
#
#SBATCH --partition=piv
#SBATCH --job-name=primes
#SBATCH --time=10:00
#SBATCH --output=primes.%j.out
#SBATCH --ntasks=4
#

echo "-------Inicio de Proceso en mpi------"
prun primes < in4
echo "-------Fin de Proceso en mpi------"
