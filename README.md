# Overview
This repo contains the supplementary materials for SESight project.

# Content
- CodeBook: This folder contains the codebook for this project. Each file refers to a participant's behavior during the experiment, and every line contains the timestamp, task name, and the action.
* Tutorial: This folder contains the tutorials for API users and GUI users. Both tutorials include guidance on the tool, as well as explanations and solutions for three practice tasks. These are expected to help users get familiar with the tools before starting the formal task.
* Task: This folder contains the source code of every task we used in both pilot study and formal experiment. Task1 and 2 were for pilot study usage while Task3 was the formal experiment task.
  - Task1: This is a 99-lines code modified from CVE-2019-25013, which contains a buffer over-read vulnerability.
  * Task2: This is a 136-lines code CVE-2021-20232, which has a use-after-free issue.
  + Task3: This is the formal experiment task, which was modified from CVE-2020-25632.
* Full Task Description: This file includes the full task description for both API users and GUI users.
* Pre-survey: This file contains the pre-survey we used to filter the participants, from which the informed consent statement, experiment explanation, and contact questions have been excluded (from the original version).
+ Post Survey: This file includes the SUS survey that participants were asked to complete after they finished the task, regardless of whether they found the vulnerability or not.
