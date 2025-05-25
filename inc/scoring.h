#ifndef __SCORING_H_
#define __SCORING_H_ 1

#include <beatmap.h>

#define MAX_SCORE       1000000
#define SCORE_MISS      0
#define SCORE_MEH       50
#define SCORE_OK        100
#define SCORE_GOOD      200
#define SCORE_GREAT     300
#define SCORE_PERFECT   305

typedef struct
{
    int combo;
    int max_combo;
    float combo_ps;
    float accuracy;
    int total_score;
    int numMiss, numMeh, numOk, numGood, numGreat, numPerfect;
} score_t;

typedef struct
{
    float max, avg, min;
    float value;
} scoring_hitwindow_t;

typedef enum
{
    JudgementNone,
    JudgementMiss,
    JudgementMeh,
    JudgementOk,
    JudgementGood,
    JudgementGreat,
    JudgementPerfect,
} scoring_judgement_type_t;

typedef struct
{
    scoring_hitwindow_t perfect;
    scoring_hitwindow_t great;
    scoring_hitwindow_t good;
    scoring_hitwindow_t ok;
    scoring_hitwindow_t meh;
    scoring_hitwindow_t miss;
} scoring_criteria_t;

void score_calculator_init(score_t* scoreObj);
void score_calculator_clear();
void score_calculator_set_difficulty(float difficulty);
int score_calculator_should_be_considered(int time, float hit_time);
int score_calculator_judge(beatmap_hitobject_t* hitobject, float hit_time);
void score_calculator_judge_as(scoring_judgement_type_t judgement);

#endif
