# Overview
This repo contains the supplementary materials for for the paper "I Can SE Clearly Now: Investigating the Effectiveness of GUI-based Symbolic Execution for Software Vulnerability Discovery."

# Content
* CodeBook: This file includes the qualitative codes for participants’ behavior.
* Tutorial: This folder contains the tutorials for API users and GUI users. Both tutorials include guidance on the tool, as well as explanations and solutions for three practice tasks. These are expected to help users get familiar with the tools before starting the formal task.
* Task: This folder contains the source code of every task we used in both pilot study and formal experiment. Task1 and 2 were for pilot study usage while Task3 was the formal experiment task.
  - Task1: This is a 99-lines code modified from CVE-2019-25013, which contains a buffer over-read vulnerability.
  * Task2: This is a 136-lines code modifiedn from CVE-2021-20232, which has a use-after-free issue.
  + Task3: This is the formal experiment task, which was modified from CVE-2020-25632.
* Full Task Description: This file includes the full task description for both API users and GUI users.
* Pre-survey: This file contains the pre-survey we used to filter the participants, from which the informed consent statement, experiment explanation, and contact questions have been excluded (from the original version).
* Pre-screening_task: This is the code for the pre-screening task. Participants were asked to find the vulnerability and describe it in the pre-survey.
* Task Process Questionnaire: This file includes the full questionnaire that participants completed after the formal vulnerability-finding task.
* Exit Survey: This file includes two parts of the questionnaire, the SUS and the SE tool feedback questions. Participants were asked to complete the entire survey regardless of whether they successfully found the vulnerability.
* Recruitment_message: This file includes the recruitment message we sent.
