# Euclidean Freeze-Tag Problem

This repository contains code for computing the **makespan (wakeup time)** of various instances of the **Euclidean Freeze-Tag Problem (E-FTP)** using algorithms proposed in the paper:

> **"Improved Wake-Up Time For Euclidean Freeze-Tag Problem"**  
> *Accepted at CCCG 2025 Conference*

## Overview

The goal is to compute the **maximum wake-up time (makespan)** across all possible configurations of Euclidean FTP instances. The code evaluates the wake-up time using optimized algorithms discussed in the paper.

## Experiment Setup

We conducted experiments under two configurations based on the value of `r₃`:

1. **Case 1**: `r₃ < 0.66`  
   - The upper bound of the `r₃` loop is set to `0.66`.

2. **Case 2**: `r₃ ≥ 0.66`  
   - The lower bound of the `r₃` loop is set to `max(r₂, 0.66)`.

In both cases, the code was executed in parallel over intervals of `r₁` in 0.1-length ranges. Each run computes the wake-up time for a specific parameter range using the format:

Format: (r₁, r₂, r₃, μ₁₂, μ₁₃, wakeup_time)

---

## Experimental Logs

### Case 1: `r₃ < 0.66`

| Interval     | Parameters                                     | Wake-Up Time |
|--------------|------------------------------------------------|---------------|
| 0.00–0.11    | (0.110, 0.110, 0.110, 3.140, 0.000, **4.2390**) |
| 0.11–0.22    | (0.210, 0.210, 0.220, 2.760, 0.000, **4.2698**) |
| 0.22–0.33    | (0.290, 0.290, 0.290, 2.420, 0.000, **4.2700**) |
| 0.33–0.44    | (0.360, 0.450, 0.450, 2.930, 0.000, **4.2700**) |
| 0.44–0.55    | (0.500, 0.510, 0.610, 1.480, 0.000, **4.2700**) |
| 0.55–0.66    | (0.560, 0.560, 0.560, 2.090, 2.090, **4.2773**) |

### Case 2: `r₃ ≥ 0.66`

| Interval     | Parameters                                     | Wake-Up Time |
|--------------|------------------------------------------------|---------------|
| 0.00–0.11    | (0.000, 0.830, 0.920, 0.000, 0.000, **4.2699**) |
| 0.11–0.22    | (0.110, 0.830, 0.920, 0.000, 0.000, **4.2699**) |
| 0.22–0.33    | (0.220, 0.830, 0.920, 0.000, 0.000, **4.2699**) |
| 0.33–0.44    | (0.330, 0.830, 0.920, 0.000, 0.000, **4.2699**) |
| 0.44–0.55    | (0.505, 0.665, 0.675, 3.010, 0.000, **4.2700**) |
| 0.55–0.66    | (0.595, 0.765, 0.820, 3.050, 0.000, **4.2700**) |
| 0.66–0.77    | (0.665, 0.745, 0.780, 2.470, 2.280, **4.2700**) |
| 0.77–0.88    | (0.770, 0.785, 0.845, 2.540, 2.340, **4.2700**) |

> **Note**: For `r₁ > 0.88`, the upper bound of **4.27** is consistently achieved, so we do not consider those ranges in our experiments.

---

## Result Summary

- The **maximum wake-up time** achieved was **4.2773**, found in the interval:

  0.55–0.66: (0.560, 0.560, 0.560, 2.090, 2.090, 4.2773)
  
- This result is also reported in **Appendix B** of the paper.

---

## System Specifications

- **CPU**: 8 cores of Intel Xeon Gold 6230  
- **RAM**: 32 GB  
- **Total Runtime**: ~40 hours
