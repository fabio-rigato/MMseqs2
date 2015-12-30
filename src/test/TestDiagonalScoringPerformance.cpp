#include <iostream>
#include <list>
#include <algorithm>
#include <math.h>
#include <SequenceLookup.h>
#include <SubstitutionMatrix.h>
#include <DiagonalMatcher.h>
#include <QueryScore.h>
#include <ExtendedSubstitutionMatrix.h>

#include "kseq.h"
#include <unistd.h> // read

KSEQ_INIT(int, read)


#include "Clustering.h"
#include "SetElement.h"

#include "DBReader.h"
#include "DBWriter.h"


int main(int argc, char **argv)
{

    size_t kmer_size = 6;
    SubstitutionMatrix subMat("/Users/mad/Documents/workspace/mmseqs/data/blosum62.out",
                              8.0, 0.0);
    SubstitutionMatrix::print(subMat.subMatrix,subMat.int2aa,subMat.alphabetSize);

    std::string S1 = "AYIAKQRQISFVKSHFSRQLEERLGLIEVQAPILSRVGDGTQDNLSGAEKAVQVKVKALPDAQFEVVHSLAKWKRQTLGQHDFSAGEGLYTHMKALRPDEDRLSPLHSVYVDQWDWERVMGDGERQFSTLKSTVEAIWAGIKATEAAVSEEFGLAPFLPDQIHFVHSQELLSRYPDLDAKGRERAIAKDLGAVFLVGIGGKLSDGHRHDVRAPDYDDWSTPSELGHAGLNGDILVWNPVLEDAFELSSMGIRVDADTLKHQLALTGDEDRLELEWHQALLRGEMPQTIGGGIGQSRLTMLLLQLPHIGQVQAGVWPAAVRESVPSLL";
    const char* S1char = S1.c_str();
//    std::cout << S1char << "\n\n";
    Sequence s1(10000, subMat.aa2int, subMat.int2aa, 0, kmer_size, true);
    s1.mapSequence(0,0,S1char);

    std::string S2 = "MLKIRYSSAFKKDLKPFQHDKSAISVINTVLKLLATGKPLPREYKEHSLKGDYIGYLECHGKPDLLLIYKRTEQEVFLYRVGSHAKLF";
    const char* S2char = S2.c_str();
//    std::cout << S2char << "\n\n";
    Sequence s2(10000, subMat.aa2int, subMat.int2aa, 0, kmer_size, true);
    s2.mapSequence(0,0,S2char);

    FILE *fasta_file = Util::openFileOrDie("/Users/mad/Documents/databases/mmseqs_benchmark/benchmarks/clustering_benchmark/db/db_full.fas", "r", true);
    kseq_t *seq = kseq_init(fileno(fasta_file));
    size_t dbEntrySize = 0;
    size_t dbCnt = 0;
    while (kseq_read(seq) >= 0) {
        dbEntrySize += seq->seq.l;
        dbCnt += 1;
    }
    SequenceLookup lookup(dbCnt*10, dbEntrySize*10);
    //kseq_destroy(seq);
    Sequence dbSeq(40000, subMat.aa2int, subMat.int2aa, 0, kmer_size, true);
    size_t id = 0;
    size_t maxLen = 0;
    for(size_t i = 0; i < 10; i++){
        fclose(fasta_file);
        fasta_file = Util::openFileOrDie("/Users/mad/Documents/databases/mmseqs_benchmark/benchmarks/clustering_benchmark/db/db_full.fas", "r", true);
        kseq_rewind(seq);
        while (kseq_read(seq) >= 0) {
            dbSeq.mapSequence(id,id,seq->seq.s);
            maxLen = std::max(seq->seq.l, maxLen);
//        if(id == 202423){
//            std::cout << seq->seq.s << std::endl;
//        }
            lookup.addSequence(&dbSeq);

            id += 1;
        }
    }
    kseq_destroy(seq);
    std::cout << maxLen << std::endl;
    DiagonalMatcher matcher(maxLen, &subMat, &lookup);
    hit_t hits[16000];
    hits[0].seqId =142424;
    hits[0].diagonal = 50;
    hits[1].seqId = 191382;
    hits[1].diagonal = 4;
    hits[2].seqId = 135950;
    hits[2].diagonal = 4;
    hits[3].seqId = 63969;
    hits[3].diagonal = 4;
    hits[4].seqId = 244188;
    hits[4].diagonal = 4;

    for(size_t i = 5; i < 16; i++) {
        hits[i].seqId = 159147;
        hits[i].diagonal = 31;
    }

    float * compositionBias = new float[s1.L];
    SubstitutionMatrix::calcLocalAaBiasCorrection(&subMat, s1.int_sequence, s1.L, compositionBias);



    matcher.processQuery(&s1,compositionBias, std::make_pair(hits, 16));
    std::cout << (int)hits[0].diagonalScore << " ";
    std::cout << (int)hits[1].diagonalScore << " ";
    std::cout << (int)hits[2].diagonalScore << " ";
    std::cout << (int)hits[3].diagonalScore << std::endl;

    matcher.processQuery(&s1, compositionBias, std::make_pair(hits, 1));
    matcher.processQuery(&s1, compositionBias, std::make_pair(hits + 1, 1));
    matcher.processQuery(&s1, compositionBias, std::make_pair(hits + 2, 1));
    matcher.processQuery(&s1, compositionBias, std::make_pair(hits + 3, 1));

    std::cout << (int)hits[0].diagonalScore<< " ";
    std::cout << (int)hits[1].diagonalScore<< " ";
    std::cout << (int)hits[2].diagonalScore<< " ";
    std::cout << (int)hits[3].diagonalScore<< std::endl;
    for(size_t i = 0; i < 10000; i++){
        for(int j = 1; j < 16000; j++){
            hits[j].seqId = rand()%dbCnt;
            hits[j].diagonal =  rand()%s1.L;
        }
        //   std::reverse(hits, hits+1000);
        matcher.processQuery(&s1, compositionBias, std::make_pair(hits, 16000));
    }
//    std::cout << ExtendedSubstitutionMatrix::calcScore(s1.int_sequence, s1.int_sequence,s1.L, subMat.subMatrix) << " " << (int)hits[0].diagonalScore <<  std::endl;
//    std::cout << (int)hits[0].diagonalScore <<  std::endl;
    for(int i = 0; i < 1000; i++){
        std::cout << hits[i].seqId << "\t" << (int) hits[i].diagonal  << "\t" << (int)hits[i].diagonalScore <<  std::endl;
    }
    delete [] compositionBias;
}
