/* This is protoype code for Hartigan's implementation of kmeans
 * WHat's promising about his implementation is that it's possible to avoid
 * giving the program the value of k. Traditionally this has been a weakness,
 * so it's interesting to try his version out.
 * I first start with his canonical clustering of food types and 3 of their characteristics.
 *
 * Unfortunately the names of each foodstuff must be two letters only
 * and they must have only three characteristics.
 *
 * IN truth, this was coded some time ago outside of git, and this version
 * is based on what was previsouly called rea3.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define GB 4
#define NSZ 2

typedef struct /* vitdats_t, a convenience function, just to bunch the vital details of the dataset together */
{
    size_t m /* total number of cases/foodpoints*/, k /* number of clusters */, n /* number of variables/dimension*/, nl /* name/label length */;
} vitdats_t;

typedef struct /* c_t, this represents 1 cluster: we'll have a k-sized array of these */
{
    char **ns;
    size_t bu; /* buffer, for the number of elements in this cluster */
    size_t nsnum; /* the actual number of cases in this cluster */
    size_t *nsi; /* array of size nsnum, containing the absolute index of the f_t case in this cluster */
    float *means;
} c_t;

typedef struct /* f_t, the struct for a foodtype ...ie. a foodpoint */
{
    char *n; /* name of the food */
    float *nv; /* values for each of the n dimensions */
    float d2l; /* euclidian distance to cluster mean */
    float eil;/* error if lost from its current cluster */
    int c; /* the cluster it's currently associated with */
} f_t; /* food type */

void printclumeans(c_t *b, vitdats_t *vt, size_t changecounter)
{
    int i, j;
    printf("Cluster means at changecounter %zu:\n", changecounter); 
    for(i=0;i<vt->k;++i) {
        for(j=0;j<vt->n; ++j)
            printf("%f ", b[i].means[j]);
        printf("\n"); 
    }
}

void printclumembs(c_t *b, vitdats_t *vt)
{
    int i, j;
    for(i=0;i<vt->k;++i) {
        printf("%i: ", i+1); 
        for(j=0; j<b[i].nsnum; ++j)
            printf("%s (idx %zu) ", b[i].ns[j], b[i].nsi[j]);
        printf("\n"); 
    }
    return;
}

void recalc2means(f_t *cases, c_t *b, vitdats_t *vt, size_t clulostidx, size_t cluwonidx, size_t affectedpt) /* when there's only 2 things you want to recalculate means on */
{
    int j, j2;
    float d;

    /* first for the cluster that lost a point */
    for(j2=0;j2<vt->n; ++j2) {
        b[clulostidx].means[j2] = 0.;
        for(j=0;j<b[clulostidx].nsnum;++j)
            b[clulostidx].means[j2] += cases[b[clulostidx].nsi[j]].nv[j2];
        b[clulostidx].means[j2] /= b[clulostidx].nsnum;
    }
    /* second for the cluster that won a point */
    for(j2=0;j2<vt->n; ++j2) {
        b[cluwonidx].means[j2] = 0.;
        for(j=0;j<b[cluwonidx].nsnum;++j)
            b[cluwonidx].means[j2] += cases[b[cluwonidx].nsi[j]].nv[j2];
        b[cluwonidx].means[j2] /= b[cluwonidx].nsnum;
    }
    /* now fill in the d2l for each of the affected point */
    cases[affectedpt].d2l = 0.;
    for(j2=0;j2<vt->n; ++j2) {
        d = cases[affectedpt].nv[j2] - b[cases[affectedpt].c-1].means[j2];
        cases[affectedpt].d2l += d*d;
    }
    cases[affectedpt].eil = (b[cases[affectedpt].c-1].nsnum * cases[affectedpt].d2l) / (b[cases[affectedpt].c-1].nsnum -1);

    return;
}

void recalcmeans(f_t *cases, c_t *b, vitdats_t *vt)
{
    int i, j, j2;
    float d;
    for(i=0;i<vt->k;++i)
        for(j2=0;j2<vt->n; ++j2) {
            b[i].means[j2] = 0.0;
            for(j=0;j<b[i].nsnum;++j)
                b[i].means[j2] += cases[b[i].nsi[j]].nv[j2];
            b[i].means[j2] /= b[i].nsnum;
        }
    /* now fill in the d2l and eil for each of the m foodpoints */
    for(i=0;i<vt->m;++i) {
        for(j2=0;j2<vt->n; ++j2) {
            d = cases[i].nv[j2] - b[cases[i].c-1].means[j2];
            cases[i].d2l += d*d;
        }
        /* the error calc is tricky ... it's soemthing that belongs to the point itself. */
        cases[i].eil = (b[cases[i].c-1].nsnum * cases[i].d2l) / (b[cases[i].c-1].nsnum -1);
    }
    return;
}

float calctote(f_t *cases, c_t *b, vitdats_t *vt)
{
    int i, j;
    float e, tote=.0;
    printf("\n"); 
    for(i=0; i<vt->m; ++i) {
        for(j=0; j<vt->n; ++j) {
            e = cases[i].nv[j] - b[cases[i].c-1].means[j];
            printf("%f ", e*e);
            tote += e*e;
        }
        printf("\n"); 
    }
    return tote;
}

f_t *creaf_t(vitdats_t *vt, size_t gbuf)
{
    int i;
    f_t *ca=malloc(gbuf*sizeof(f_t));
    for(i=0;i<gbuf;++i) {
        ca[i].n=calloc(vt->nl+1,sizeof(char));
        ca[i].nv=calloc(vt->n, sizeof(float));
        ca[i].d2l=0.0;
        ca[i].eil=0.0;
    }
    return ca;
}

c_t *creac_t(vitdats_t *vt) /* simply uses k to set up our array of clusters */
{
    int i, j;
    c_t *c=malloc(vt->k*sizeof(c_t));
    for(i=0;i<vt->k;++i) {
        c[i].bu=GB;
        c[i].nsnum=0;
        c[i].means=malloc(vt->n*sizeof(float)); /* no need to set to set to zero on creation, recalcmeans will take care of that */
        c[i].nsi=malloc(c[i].bu*sizeof(size_t));
        c[i].ns=malloc(c[i].bu*sizeof(char *));
        for(j=0;j<c[i].bu;++j) 
            c[i].ns[j]=calloc(vt->nl+1,sizeof(char));
    }
    return c;
}

void freef_t(f_t *ca, vitdats_t *vt)
{
    int i;
    for(i=0;i<vt->m;++i) {
        free(ca[i].n);
        free(ca[i].nv);
    }
    free(ca);
}

void freec_t(c_t *c, vitdats_t *vt)
{
    int i, j;
    for(i=0;i<vt->k;++i) {
        for(j=0;j<c[i].nsnum;++j) 
            free(c[i].ns[j]);
        free(c[i].ns);
        free(c[i].nsi);
        free(c[i].means);
    }
    free(c);
}

f_t *f2struct(char *fname, vitdats_t *vt) /* cheap and unrobust way of rendering input file into f_t struct array */
{
    FILE *fp = fopen(fname, "r");
    if (fp == NULL ) {
        fprintf(stderr,"Can't open input file.");
        exit(EXIT_FAILURE);
    }

    size_t gbuf=GB;
    f_t *fl=creaf_t(vt, gbuf);

    int i=0, j;
    while( (j=fscanf(fp, "%c%c %f %f %f\n", &fl[i].n[0], &fl[i].n[1], &fl[i].nv[0], &fl[i].nv[1], &fl[i].nv[2])) == 5) {
        i++;
        if( i == gbuf-1) {
            gbuf += GB;
            fl=realloc(fl, gbuf*sizeof(f_t));
            for(j=gbuf-GB;j<gbuf;++j) {
                fl[j].n=calloc(vt->nl+1, sizeof(char));
                fl[j].nv=calloc(vt->n, sizeof(float));
                fl[j].d2l=0.0;
                fl[j].eil=0.0;
            }
        }
    }
    fclose(fp);
    vt->m=i;
    /* normalisation sequence */
    for(i=vt->m;i<gbuf;++i) {
        free(fl[i].n);
        free(fl[i].nv);
    }
    fl=realloc(fl, vt->m*sizeof(f_t));
    return fl;
}

void firstgo(f_t *cases, c_t *b, vitdats_t *vt) /* the rough first clustering scheme */
{
    int i, j;

    /* at this early stage we'll add each dimension into a a fp value: obviously horrendously inaccurate, but it's a start */
    float *casum=calloc(vt->m, sizeof(float));
    for(j=0;j<vt->n;++j) 
        casum[0] += cases[0].nv[j];
    float min, max;
    min = max = casum[0];
    for(i=1;i<vt->m;++i) {
        for(j=0;j<vt->n;++j)
            casum[i]+=cases[i].nv[j];
        if(casum[i] > max) {
            max=casum[i];
            continue;
        } else if(casum[i] < min)
            min=casum[i];
    }

    float intv=(max-min)/vt->k; /* interval, initially, we just going to group according to rough size */
    for(i=0;i<vt->m;++i)
        for(j=1;j<=vt->k;++j) 
            if(casum[i] <=min+intv*j) {
                cases[i].c = j;
                break;
            }
    /* "artificial because of Hartigan 1975 error". I think this was just a trivial error, though I dont explain myself here
     * but this hard code must specifically refer to the food stuff data set, and how I wa probably trying to get his extact results. */
    cases[4].c=3; 

    //        cases[i].c= 1+k*(casum[i]-min) / (max-min+1);

    printf("Initial clussteralloc: each point, the horrendous amalgam of distance, and the cluster is belogns to 1-indexed.\n"); 
    for(i=0;i<vt->m;++i)
        printf("%s: %f - %i.\n", cases[i].n, casum[i], cases[i].c);
    free(casum);

    /*Ok, we cycle through the points and build up the clustersn (variable b)  properly */
    /* cases[i].c-1 is basically the index of the cluster that a food point belongs to */
    for(i=0;i<vt->m;++i) {
        if(b[cases[i].c-1].nsnum == b[cases[i].c-1].bu) {
            b[cases[i].c-1].bu += GB;
            b[cases[i].c-1].nsi = realloc(b[cases[i].c-1].nsi, sizeof(size_t));
            b[cases[i].c-1].ns = realloc(b[cases[i].c-1].ns, sizeof(char *));
            for(j=b[cases[i].c-1].bu-GB; j<b[cases[i].c-1].bu; ++j) 
                b[cases[i].c-1].ns[j] = calloc(vt->nl+1, sizeof(char));
        }
        /* some mad indices here, you need to immerse yourself to "get" them */
        strcpy(b[cases[i].c-1].ns[b[cases[i].c-1].nsnum], cases[i].n);
        b[cases[i].c-1].nsi[b[cases[i].c-1].nsnum] = (size_t)i;
        b[cases[i].c-1].nsnum++;
    }
    return;
}

void cyclerun(f_t *cases, c_t *b, vitdats_t *vt, size_t *changecounter, float *toterr)
{
    int i, i3, j, j2, starti; /* assorted indices */
    char slidedown=0;
    float d, d2, minerr, newerr;
    int mindx, oldidx, iopt;
    /* received values that will change */
    float tote = *toterr;
    size_t chacou = *changecounter;

    for(j=0;j<vt->m;++j) {
        starti = cases[j].c-1; /* this is the cluster the current point belongs to */
        /* take next cluster after current one, and give it minerr and mindx */
        printf("pt. %i at %i with %f:", j, starti, cases[j].d2l);
        iopt=(starti+0+1)%vt->k;
        printf("%i-", iopt); 
        d2=0.;
        for(j2=0;j2<vt->n;++j2) {
            d = cases[j].nv[j2] - b[iopt].means[j2];
            d2+=d*d;
        }
        minerr = (b[iopt].nsnum * d2  / (b[iopt].nsnum+1)) - cases[j].eil;
        printf("%f err %f", d2, minerr); 
        mindx = iopt;
        /* withinerr and mindx happily assigned, we go through the rest */
        for(i=1;i<vt->k-1;++i) {
            iopt=(starti+i+1)%vt->k;
            printf("%i=", iopt); 
            d2=0.;
            for(j2=0;j2<vt->n;++j2) {
                d = cases[j].nv[j2] - b[iopt].means[j2];
                d2+=d*d;
            }
            newerr = (b[iopt].nsnum * d2  / (b[iopt].nsnum+1)) - cases[j].eil;
            printf("%f err %f /", d2, newerr); 
            if(minerr > newerr) {
                minerr=newerr;
                mindx = iopt;
            }
        }
        printf("\n"); 
        /* OK, is the minerr negative? We should reposition this point then */
        if( (minerr<0) && (b[cases[j].c-1].nsnum > 1) ) {
            chacou++;
            oldidx = cases[j].c-1;
            cases[j].c = mindx+1;
            printf("Clust %i with %zu will lose %s.\n", oldidx, b[oldidx].nsnum, cases[j].n); 
            printf("Clust %i with %zu will gain %s.\n", mindx, b[mindx].nsnum, cases[j].n); 
            /* but is there space for this extra point in this cluster ... check nad if not, make it */
            if(b[mindx].nsnum == b[mindx].bu) {
                b[mindx].bu += GB;
                b[mindx].nsi = realloc(b[mindx].nsi, b[mindx].bu*sizeof(size_t));
                b[mindx].ns = realloc(b[mindx].ns, b[mindx].bu*sizeof(char *));
                for(i3=b[mindx].bu-GB; i3<b[mindx].bu; ++i3) 
                    b[mindx].ns[i3] = calloc(vt->nl+1, sizeof(char));
            }
            strcpy(b[mindx].ns[b[mindx].nsnum], cases[j].n);
            b[mindx].nsi[b[mindx].nsnum] = (size_t)j;
            b[mindx].nsnum++;
            /* of course, the old cluster has lost a member however ... where was it? */
            for(slidedown=0, i3=0; i3<b[oldidx].nsnum; i3++) {
                if(j==b[oldidx].nsi[i3]) {
                    slidedown=1;
                    continue;
                }
                if(slidedown) {
                    b[oldidx].nsi[i3-1] = b[oldidx].nsi[i3];
                    strcpy(b[oldidx].ns[i3-1], b[oldidx].ns[i3]);
                }
            }
            b[oldidx].nsnum--;
            printclumembs(b, vt);
            tote += minerr;
            printf("Total error for this partitioning is %f\n", tote);
            /* And because of this change, we need to recalculate means and d2l */
            recalc2means(cases, b, vt, oldidx, mindx, j);
        }
    } /*end of outer loop */
    *changecounter = chacou;
    *toterr=tote;
    return;
}

int main(int argc, char *argv[])
{
    if(argc != 3) {
        printf("Usage: two args. 1) data file 2) your desired value of \"k\"\n");
        exit(EXIT_FAILURE);
    }

    int i, j; /* assorted indices */
    vitdats_t *vt=malloc(sizeof(vitdats_t));
    vt->n=3; /* hard coded, yes I know: this means number of features/dimensions for the data */
    vt->nl=2; /* lengths of the labels: the foods are abbreviated to two letters */
    vt->k=(size_t)atoi(argv[2]); /* number of clusters requested */
    size_t chacou=0; /* aka. changecounter */
    float tote;

    f_t *cases=f2struct(argv[1], vt);
    printf("File read in, report: %zu cases/foodpoints.\n", vt->m); 

    c_t *cts=creac_t(vt); /* cts, our array of clusters */

    firstgo(cases, cts, vt);
    printclumembs(cts, vt);

    recalcmeans(cases, cts, vt); /* actually you only need calculate the means of the changed clusters. */
    tote=calctote(cases, cts, vt);

    printf("Initial allocation:\n");
    printclumeans(cts, vt, chacou);

    /* OK, go for the cycleruns */
    for(i=0;i<3;++i) {
        cyclerun(cases, cts, vt, &chacou, &tote);
        printf("Changes occured: %zu\n", chacou);
    }

    /* now normalize memory .. */
    for(i=0;i<vt->k;++i) {
        for(j=cts[i].nsnum; j<cts[i].bu; ++j)
            free(cts[i].ns[j]);
        cts[i].nsi = realloc(cts[i].nsi, cts[i].nsnum*sizeof(size_t));
        cts[i].means = realloc(cts[i].means, cts[i].nsnum*sizeof(size_t));
    }

    freef_t(cases, vt);
    freec_t(cts, vt);
    free(vt);
    return 0;
}
