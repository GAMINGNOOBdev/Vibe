#include <scoring.h>
#include <beatmap.h>
#include <memory.h>

scoring_criteria_t score_criteria = {
    .perfect = {.max=22.4f, .avg=19.4f, .min=13.9f, .value=19.4f},
    .great = {.max=64.f, .avg=49.f, .min=34.f, .value=49.f},
    .good = {.max=97.f, .avg=82.f, .min=67.f, .value=82.f},
    .ok = {.max=127.f, .avg=112.f, .min=97.f, .value=112.f},
    .meh = {.max=151.f, .avg=136.f, .min=121.f, .value=136.f},
    .miss = {.max=188.f, .avg=173.f, .min=158.f, .value=173.f},
    .total_objects=0
};
score_t* score_object = NULL;
scoring_judgement_set_callback_t score_judgement_callback = NULL;

void score_calculator_init(score_t* scoreObj)
{
    score_object = scoreObj;
}

void score_calculator_clear(void)
{
    if (score_object == NULL)
        return;

    memset(score_object, 0, sizeof(score_t));
}

void score_calculator_set_judgement_callback(scoring_judgement_set_callback_t callback)
{
    score_judgement_callback = callback;
}

void score_calculator_apply_difficulty(scoring_hitwindow_t* hitwindow, float od)
{
    float mid = hitwindow->avg, max = hitwindow->max, min = hitwindow->min;
    float value = hitwindow->avg;
    float difficulty = (od - 5.f) / 5.f;

    if (od > 5.f)
        value = mid + (max - mid) * difficulty;
    if (od > 5.f)
        value = mid + (mid - min) * difficulty;

    hitwindow->value = value;
}

void score_calculator_set_difficulty(float difficulty)
{
    score_calculator_apply_difficulty(&score_criteria.perfect, difficulty);
    score_calculator_apply_difficulty(&score_criteria.great, difficulty);
    score_calculator_apply_difficulty(&score_criteria.good, difficulty);
    score_calculator_apply_difficulty(&score_criteria.ok, difficulty);
    score_calculator_apply_difficulty(&score_criteria.meh, difficulty);
    score_calculator_apply_difficulty(&score_criteria.miss, difficulty);
}

void score_calculator_set_total_objects(int total_objects)
{
    score_criteria.total_objects = total_objects;
}

int8_t score_calculator_check_if_time_in_range(float time, scoring_hitwindow_t* window)
{
    if (time < window->value && time > -window->value)
        return 1;

    return 0;
}

int score_calculator_is_missed(beatmap_hitobject_t hitobject, float time)
{
    float deltaT = hitobject.time - time;

    if (hitobject.isLN)
        deltaT = hitobject.end - time;

    return deltaT < -score_criteria.miss.value && !hitobject.hit;
}

int score_calculator_should_be_considered(int time, float hit_time)
{
    float t = hit_time - (float)time;
    if (t < -score_criteria.miss.value)
        return 0;

    return 1;
}

void score_calculator_calc_score(void)
{
    int totalHits = score_object->numMiss + score_object->numMeh + score_object->numOk +
                    score_object->numGood + score_object->numGreat + score_object->numPerfect;

    int weighted = SCORE_MISS * score_object->numMiss +
                   SCORE_MEH * score_object->numMeh +
                   SCORE_OK * score_object->numOk +
                   SCORE_GOOD * score_object->numGood +
                   SCORE_GREAT * score_object->numGreat +
                   SCORE_PERFECT * score_object->numPerfect;

    int accscore = SCORE_MISS * score_object->numMiss +
                   SCORE_MEH * score_object->numMeh +
                   SCORE_OK * score_object->numOk +
                   SCORE_GOOD * score_object->numGood +
                   SCORE_GREAT * score_object->numGreat +
                   SCORE_GREAT * score_object->numPerfect;

    score_object->accuracy = (float)accscore / (SCORE_GREAT * totalHits);

    score_object->total_score = (int)((float)weighted / (SCORE_PERFECT * score_criteria.total_objects) * MAX_SCORE);
}

int score_calculator_judge(beatmap_hitobject_t* hitobject, float hit_time)
{
    /// NOTE:
    /// This may or may not be a pretty hefty
    /// operation since we are handling a lot
    /// of scoring logic.
    /// Possibly try putting this off to a
    /// seperate thread including the actual
    /// handling of user input.

    if (hitobject->hit)
        return 1;

    float time = hit_time - (float)hitobject->time;

    scoring_judgement_type_t judgement = JudgementNone;
    if (score_calculator_check_if_time_in_range(time, &score_criteria.miss))
        judgement = JudgementNone; // ignore judgement if in the range of missing
    if (score_calculator_check_if_time_in_range(time, &score_criteria.meh))
        judgement = JudgementMeh;
    if (score_calculator_check_if_time_in_range(time, &score_criteria.ok))
        judgement = JudgementOk;
    if (score_calculator_check_if_time_in_range(time, &score_criteria.good))
        judgement = JudgementGood;
    if (score_calculator_check_if_time_in_range(time, &score_criteria.great))
        judgement = JudgementGreat;
    if (score_calculator_check_if_time_in_range(time, &score_criteria.perfect))
        judgement = JudgementPerfect;

    if (score_judgement_callback != NULL)
        score_judgement_callback(judgement);

    if (judgement == JudgementNone)
        return 0;
    score_calculator_judge_as(judgement);

    return 1;
}

int score_calculator_judge_release(beatmap_hitobject_t* hitobject, float hit_time)
{
    if (hitobject->tailHit)
        return 0;

    float time = hit_time - (float)hitobject->end;

    scoring_judgement_type_t judgement = JudgementNone;
    if (score_calculator_check_if_time_in_range(time, &score_criteria.miss))
        judgement = JudgementMiss;
    if (score_calculator_check_if_time_in_range(time, &score_criteria.meh))
        judgement = JudgementMeh;
    if (score_calculator_check_if_time_in_range(time, &score_criteria.ok))
        judgement = JudgementOk;
    if (score_calculator_check_if_time_in_range(time, &score_criteria.good))
        judgement = JudgementGood;
    if (score_calculator_check_if_time_in_range(time, &score_criteria.great))
        judgement = JudgementGreat;
    if (score_calculator_check_if_time_in_range(time, &score_criteria.perfect))
        judgement = JudgementPerfect;

    if (score_judgement_callback != NULL)
        score_judgement_callback(judgement);

    if (judgement == JudgementNone)
        return 0;
    score_calculator_judge_as(judgement);

    hitobject->tailHit = 1;
    return 1;
}

void score_calculator_judge_as(scoring_judgement_type_t judgement)
{
    if (score_judgement_callback != NULL)
        score_judgement_callback(judgement);

    if (judgement == JudgementMiss)
    {
        score_object->numMiss++;
        if (score_object->max_combo < score_object->combo)
            score_object->max_combo = score_object->combo;

        score_object->combo = 0;
        score_calculator_calc_score();
        return;
    }

    if (judgement == JudgementMeh)
        score_object->numMeh++;
    else if (judgement == JudgementOk)
        score_object->numOk++;
    else if (judgement == JudgementGood)
        score_object->numGood++;
    else if (judgement == JudgementGreat)
        score_object->numGreat++;
    else if (judgement == JudgementPerfect)
        score_object->numPerfect++;

    score_object->combo++;
    score_calculator_calc_score();
}
