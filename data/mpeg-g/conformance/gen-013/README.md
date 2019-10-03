# Generation of the input SAM/BAM file

Apply the following steps to the file ``wgs/e-coli/DH10B/MiSeq_Ecoli_DH10B_110721_PF.bam`` from the sequencing data collection:

1. Select only proper pairs with ``samtools view -f 2``
2. Select only those reads mapping to ``EcoliDH10B.fa:1-10``
4. Sort reads by mapping position
5. Manually introduce variable lengths

The resulting data exhibits the following properties:

- Aligned
- Variable read lengths
- Single-ended reads

# Part 2 coverage

- 10.4 and subclauses
- 11.3
- 11.3.4
