#include <scoring.h>
#include <memory.h>

scoring_criteria_t score_criteria = {
    .perfect = {.max=22.4f, .avg=19.4f, .min=13.9f, .value=19.4f},
    .great = {.max=64.f, .avg=49.f, .min=34.f, .value=49.f},
    .good = {.max=97.f, .avg=82.f, .min=67.f, .value=82.f},
    .ok = {.max=127.f, .avg=112.f, .min=97.f, .value=112.f},
    .meh = {.max=151.f, .avg=136.f, .min=121.f, .value=136.f},
    .miss = {.max=188.f, .avg=173.f, .min=158.f, .value=173.f},
};
score_t* score_object = NULL;

void score_calculator_init(score_t* scoreObj)
{
    score_object = scoreObj;
}

void score_calculator_clear()
{
    if (score_object == NULL)
        return;

    memset(score_object, 0, sizeof(score_t));
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

int8_t score_calculator_check_if_time_in_range(float time, scoring_hitwindow_t* window)
{
    if (time < window->value && time > -window->value)
        return 1;

    return 0;
}

int score_calculator_should_be_considered(int time, float hit_time)
{
    float t = hit_time - (float)time;
    if (t < -score_criteria.miss.value)
        return 0;

    return 1;
}

void score_calculator_add_score(scoring_judgement_type_t judgement)
{
    if (judgement == JudgementMiss)
        score_object->total_score += SCORE_MISS;
    if (judgement == JudgementMeh)
        score_object->total_score += SCORE_MEH;
    if (judgement == JudgementOk)
        score_object->total_score += SCORE_OK;
    if (judgement == JudgementGood)
        score_object->total_score += SCORE_GOOD;
    if (judgement == JudgementGreat)
        score_object->total_score += SCORE_GREAT;
    if (judgement == JudgementPerfect)
        score_object->total_score += SCORE_PERFECT;
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

    if (judgement == JudgementNone)
        return 0;
    score_calculator_judge_as(judgement);

    // score calculation and combo handling here !!!
    score_calculator_add_score(judgement);

    return 1;
}

void score_calculator_judge_as(scoring_judgement_type_t judgement)
{
    if (judgement == JudgementMiss)
        score_object->numMiss++;
    if (judgement == JudgementMeh)
        score_object->numMeh++;
    if (judgement == JudgementOk)
        score_object->numOk++;
    if (judgement == JudgementGood)
        score_object->numGood++;
    if (judgement == JudgementGreat)
        score_object->numGreat++;
    if (judgement == JudgementPerfect)
        score_object->numPerfect++;
}
