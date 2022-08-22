#!/bin/bash

JOBS_FILE="jobs_ny_sf_sy-u.sh"

if [ -r $JOBS_FILE ]; then
    echo "file '$JOBS_FILE' exists, exiting ..."
    exit
fi

echo '#!/bin/bash' > $JOBS_FILE
echo '#SBATCH -N 1' >> $JOBS_FILE
echo '#SBATCH -t 48:00:00' >> $JOBS_FILE
echo '#SBATCH --mem=90G' >> $JOBS_FILE
echo '#SBATCH --mail-type=BEGIN,END' >> $JOBS_FILE
echo '#SBATCH --mail-user=a.de.macedo.florio@tue.nl' >> $JOBS_FILE
echo 'RUNPATH=$HOME/hga-dial-a-ride/src' >> $JOBS_FILE
echo 'cd $RUNPATH' >> $JOBS_FILE

INSTANCES="ny sf sy-u"
REQS="2000 4000 6000 8000 10000"
DRIVERS="30 60 90 120 150"
CAPACITIES="2 4 8 16 32 64"

for inst in $INSTANCES; do
    for req in $REQS; do
        for drvs in $DRIVERS; do
            for cap in $CAPACITIES; do
                echo "\$RUNPATH/main $inst $req $drvs $cap > \$RUNPATH/../out/$inst-$req-$drvs-$cap.out &" >> $JOBS_FILE
                echo 'wait -n' >> $JOBS_FILE
            done
        done
    done
done

