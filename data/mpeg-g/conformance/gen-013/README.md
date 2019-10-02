# Generation of the input SAM/BAM file

Apply the following steps to the file ``wgs/h-sapiens/NA12878/NA12878.pacbio.bwa-sw.20140202.bam`` from the sequencing data collection:

1. Select only those reads mapping to ``1:10000-11000``
2. Select only primary alignments with ``samtools -F 256``
3. Sort reads by mapping position
4. Clean up some minor things manually

The resulting data exhibits the following properties:

- Aligned
- Variable read lengths
- Single-ended reads

# Part 2 coverage

- 10.4 and subclauses
- 11.3
- 11.3.4
