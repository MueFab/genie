# Generation of the input SAM/BAM file

Apply the following steps to the file ``wgs/e-coli/DH10B/MiSeq_Ecoli_DH10B_110721_PF.bam`` from the sequencing data collection:

1. Select only proper pairs with ``samtools view -f 2``
2. Select only those reads mapping to ``EcoliDH10B.fa:1-10``
3. Also select the mates belonging to the reads from step 2
4. Sort reads by mapping position
5. Clean up some minor things manually

The resulting data exhibits the following properties:

- Aligned
- Fixed read lengths
- Paired-end reads

# Part 2 coverage

- 10.4 and subclauses
- 11.3
- 11.3.4
