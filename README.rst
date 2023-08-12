========
Overview
========

.. start-badges

.. list-table::
    :stub-columns: 1

    * - tests
      - | |github-actions|
        |
    * - package
      - | |commits-since|

.. |github-actions| image:: https://github.com/ebi-metagenomics/combined-gene-caller/actions/workflows/github-actions.yml/badge.svg
    :alt: GitHub Actions Build Status
    :target: https://github.com/ebi-metagenomics/combined-gene-caller/actions

.. |commits-since| image:: https://img.shields.io/github/commits-since/ebi-metagenomics/combined-gene-caller/v0.1.0.svg
    :alt: Commits since latest release
    :target: https://github.com/ebi-metagenomics/combined-gene-caller/compare/v0.1.0...main



.. end-badges

Combined gene caller for EMG pipeline, to combine predictions of FragGeneScan, prodigal, etc.

* Free software: Apache Software License 2.0

Installation
============

::

    pip install combined-gene-caller

You can also install the in-development version with::

    pip install https://github.com/ebi-metagenomics/combined-gene-caller/archive/main.zip


Documentation
=============


To use the project:

.. code-block:: python

    import combined_gene_caller
    combined_gene_caller.longest()


Development
===========

To run all the tests run::

    tox

Note, to combine the coverage data from all the tox environments run:

.. list-table::
    :widths: 10 90
    :stub-columns: 1

    - - Windows
      - ::

            set PYTEST_ADDOPTS=--cov-append
            tox

    - - Other
      - ::

            PYTEST_ADDOPTS=--cov-append tox
